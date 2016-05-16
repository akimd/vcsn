#pragma once

#include <algorithm>
#include <deque>
#include <fstream>

#include <boost/heap/binomial_heap.hpp>
#include <boost/optional.hpp>

#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/word-polynomialset.hh>

#include <vcsn/algos/lightest-path.hh>
#include <vcsn/algos/is-acyclic.hh>

namespace vcsn
{

  /*----------------------.
  | shortest(automaton).  |
  `----------------------*/

  namespace detail
  {
    /// Compute the shortest words accepted by an automaton.
    template <Automaton Aut>
    class enumerater
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<Aut>;

      using wordset_context_t = word_context_t<context_t>;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      /// Wordset.
      using labelset_t = labelset_t_of<polynomialset_t>;
      using word_t = word_t_of<automaton_t>;

      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      /// Used in the case of non-free labelsets.
      using profile_t = std::tuple<state_t, word_t, weight_t>;
      struct profile_less
      {
        /// Whether l < r (as this is a max heap).
        bool operator()(const profile_t& r, const profile_t& l) const
        {
          if (labelset_t::less(std::get<1>(l), std::get<1>(r)))
            return true;
          else if (labelset_t::less(std::get<1>(r), std::get<1>(l)))
            return false;
          else
            return std::get<0>(l) < std::get<0>(r);
        }
      };

      /// Prepare to compute an approximation of the behavior.
      ///
      /// \param aut   the automaton to approximate
      enumerater(const automaton_t& aut)
        : aut_(aut)
      {}

      /// The approximated behavior of the automaton.
      /// \param num   number of words looked for.
      /// \param len   maximum length of words looked for.
      polynomial_t operator()(boost::optional<unsigned> num,
                              boost::optional<unsigned> len)
      {
        if (!num)
          num = !len ? 1 : std::numeric_limits<unsigned>::max();
        if (!len)
          len = std::numeric_limits<unsigned>::max();

        // If the user did not specify a maximum length, and required only one
        // path, a lightest-path algorithm is eligible.
        if (*len == std::numeric_limits<unsigned>::max()
            && (*num == 1 || is_acyclic(aut_)))
          {
            auto mul = [this](auto lhs, transition_t_of<Aut> t)
                             {
                               auto rhs = aut_->label_of(t);
                               return (aut_->labelset()->is_special(rhs))
                                      ? lhs
                                      : ps_.labelset()->mul(lhs, aut_->label_of(t));
                             };
            auto get_value = [](auto m) { return m.first; };
            auto yen = detail::make_yen(aut_, *ps_.labelset(), mul, get_value);
            auto paths = yen(aut_->pre(), aut_->post(), *num);

            using context_t = context_t_of<Aut>;
            using wordset_context_t = detail::word_context_t<context_t>;
            using polynomialset_t = polynomialset<wordset_context_t>;
            using polynomial_t = typename polynomialset_t::value_t;
            const polynomialset_t ps = make_word_polynomialset(aut_->context());
            polynomial_t res;
            for (const auto& path : paths)
              {
                auto monomial = path_monomial(aut_, format_lightest(aut_, path));
                if (monomial)
                  ps.add_here(res, *monomial);
              }
            return res;
          }
        else
          return shortest_(*num, *len);
      }

    private:
      /// Case of free labelsets (e.g., `lal` or `lal x lal`).
      ///
      /// We maintain a list of current tuples of (state, label, weight).
      /// During one round we pass them all through outgoing
      /// transitions, which gives the next list of (state, label, weight).
      ///
      /// Each round contributes one letter to all the labels, so
      /// once we ran `len` rounds, we have all the words shorter than
      /// `len` letters.
      template <typename LabelSet = labelset_t_of<context_t>>
      auto shortest_(unsigned num, unsigned len)
        -> std::enable_if_t<LabelSet::is_free(), polynomial_t>
      {
        // Each step of propagation contributes a letter.  We need to
        // take the initial and final special characters into account.
        if (len != std::numeric_limits<unsigned>::max())
          len += 2;

        using queue_t = std::deque<profile_t>;
        auto queue = queue_t{profile_t{aut_->pre(), ls_.one(), ws_.one()}};

        // The approximated behavior: the first orders to post's past.
        polynomial_t output;
        for (unsigned i = 0;
             !queue.empty() && i < len && output.size() < num;
             ++i)
          {
            queue_t q2;
            for (const auto& sm: queue)
              {
                state_t s; word_t l; weight_t w;
                std::tie(s, l, w) = sm;
                for (const auto t: all_out(aut_, s))
                  {
                    auto dst = aut_->dst_of(t);
                    auto nw = ws_.mul(w, aut_->weight_of(t));
                    if (aut_->src_of(t) == aut_->pre())
                      q2.emplace_back(dst, l, std::move(nw));
                    else if (dst == aut_->post())
                      ps_.add_here(output, l, std::move(nw));
                    else
                      q2.emplace_back(dst,
                                      ls_.mul(l, aut_->label_of(t)),
                                      std::move(nw));
                  }
              }
            queue.swap(q2);
          }

        polynomial_t res;
        for (const auto& m: output)
          {
            ps_.add_here(res, m);
            if (--num == 0)
              break;
          }
        return res;
      }

      /// Case of non free labelsets (e.g., `law`, `lan x lan`).
      ///
      /// We use a unique queue composed of (state, word, weight),
      /// shortest words first.
      template <typename LabelSet = labelset_t_of<context_t>>
      auto shortest_(unsigned num, unsigned len)
        -> std::enable_if_t<!LabelSet::is_free(), polynomial_t>
      {
        // Benched as better than Fibonacci, Pairing and Skew Heaps.
        // D-ary heaps and Priority Queue fail to compile.
        using queue_t =
          boost::heap::binomial_heap<profile_t,
                                     boost::heap::compare<profile_less>>;

        auto queue = queue_t{};
        queue.emplace(aut_->pre(), ls_.one(), ws_.one());

        // The approximated behavior: the first orders to post's past.
        polynomial_t res;
        while (!queue.empty())
          {
            state_t s; word_t l; weight_t w;
            std::tie(s, l, w) = queue.top();

            // Take all the top of the queue if they have the same
            // label and state: sum the weights.
            //
            // Benches show that this is way more efficient than
            // trying to update matching profile_t in the queue, even if
            // we try to take advantage of the ordering in the heap.
            // Here, we benefit from the fact that the queue provides
            // matching profile_t in a row.
            queue.pop();

            while (!queue.empty()
                   && std::get<0>(queue.top()) == s
                   && ls_.equal(std::get<1>(queue.top()), l))
              {
                w = ws_.add(w, std::get<2>(queue.top()));
                queue.pop();
              }

            for (const auto t: all_out(aut_, s))
              {
                auto dst = aut_->dst_of(t);
                auto nw = ws_.mul(w, aut_->weight_of(t));
                if (aut_->src_of(t) == aut_->pre())
                  queue.emplace(dst, l, std::move(nw));
                else if (dst == aut_->post())
                  ps_.add_here(res, l, std::move(nw));
                else
                  {
                    auto nl = ls_.mul(l, aut_->label_of(t));
                    // Discard candidates that are too long.
                    if (ls_.size(nl) <= len)
                      queue.emplace(dst, std::move(nl), std::move(nw));
                  }
              }

            // If we found enough words *and* we have completely
            // treated the current word (there are no other copies in
            // other states), we're done.
            if (queue.empty()
                || (num == res.size()
                    && !ls_.equal(std::get<1>(queue.top()), l)))
              break;
          }

        return res;
      }

    private:
      /// Show the heap, for debugging.
      template <typename Queue>
      void show_heap_(const Queue& q, std::ostream& os = std::cerr) const
      {
        const char* sep = "";
        for (auto i = q.ordered_begin(), end = q.ordered_end();
             i != end; ++i)
          {
            os << sep;
            sep = ", ";
            aut_->print_state_name(std::get<0>(*i), os) << ":<";
            ws_.print(std::get<2>(*i), os) << '>';
            ls_.print(std::get<1>(*i), os);
          }
        os << '\n';
      }

      /// The automaton whose behavior to approximate.
      automaton_t aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();
      /// Shorthand to the polynomialset of words.
      const polynomialset_t ps_ = make_word_polynomialset(aut_->context());
      /// Shorthand to the (word) labelset.
      const labelset_t& ls_ = *ps_.labelset();
    };
  }

  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  /// \param num   number of words looked for.
  /// \param len   maximum length of words looked for.
  template <Automaton Aut>
  typename detail::enumerater<Aut>::polynomial_t
  shortest(const Aut& aut,
           boost::optional<unsigned> num = {},
           boost::optional<unsigned> len = {})
  {
    auto enumerater = detail::enumerater<Aut>{aut};
    return enumerater(num, len);
  }


  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  /// \param len   maximum length of words looked for.
  template <Automaton Aut>
  typename detail::enumerater<Aut>::polynomial_t
  enumerate(const Aut& aut, unsigned len)
  {
    return shortest(aut, boost::none, len);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Num, typename Len>
      polynomial
      shortest(const automaton& aut,
               boost::optional<unsigned> num,
               boost::optional<unsigned> len)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a->context());
        return {ps, shortest(a, num, len)};
      }
    }
  }
}
