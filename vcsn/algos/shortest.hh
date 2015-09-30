#pragma once

#include <algorithm>
#include <deque>
#include <fstream>

#include <boost/optional.hpp>

#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/labelset/word-polynomialset.hh>

namespace vcsn
{

  /*----------------------.
  | shortest(automaton).  |
  `----------------------*/

  namespace detail
  {
    template <typename Aut>
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
      using datum_t = std::tuple<state_t, word_t, weight_t>;
      struct datum_less
      {
        /// Whether l < r (as this is a max heap).
        bool operator()(const datum_t& r, const datum_t& l) const
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

        return shortest_(*num, *len);
      }

    private:
      /// Case of free labelsets (e.g., lal or lal x lal).
      ///
      /// We maintain a list of current pairs of (state, monomial).
      /// During one round we pass them all through outgoing
      /// transitions, which gives the next list of (state, monomial).
      ///
      /// Each round contributes one letter to all the monomials, so
      /// once we ran len rounds, we have all the words shorter than
      /// len letters.
      template <typename LabelSet = labelset_t_of<context_t>>
      auto shortest_(unsigned num, unsigned len)
        -> enable_if_t<LabelSet::is_free(), polynomial_t>
      {
        // Each step of propagation contributes a letter.  We need to
        // take the initial and final special characters into account.
        if (len != std::numeric_limits<unsigned>::max())
          len += 2;

        using monomial_t = typename polynomialset_t::monomial_t;
        using queue_t = std::deque<std::pair<state_t, monomial_t>>;

        auto queue = queue_t{{aut_->pre(), ps_.monomial_one()}};
        // The approximated behavior: the first orders to post's past.
        polynomial_t output;
        for (unsigned i = 0;
             !queue.empty() && i < len && output.size() < num;
             ++i)
          {
            queue_t q2;
            for (const auto& sm: queue)
              {
                state_t s = sm.first;
                const monomial_t& m = sm.second;
                for (const auto t: aut_->all_out(s))
                  {
                    auto dst = aut_->dst_of(t);
                    auto nw = ws_.mul(weight_of(m), aut_->weight_of(t));
                    if (aut_->src_of(t) == aut_->pre())
                      q2.emplace_back(dst,
                                      monomial_t{label_of(m), std::move(nw)});
                    else if (dst == aut_->post())
                      ps_.add_here(output, label_of(m), std::move(nw));
                    else
                      q2.emplace_back(dst,
                                      monomial_t{ls_.mul(label_of(m),
                                                         aut_->label_of(t)),
                                                 std::move(nw)});
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

    private:
      /// The automaton whose behavior to approximate.
      const automaton_t& aut_;
      const weightset_t& ws_ = *aut_->weightset();
      const polynomialset_t ps_ = make_word_polynomialset(aut_->context());
      const labelset_t& ls_ = *ps_.labelset();
    };
  }

  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  /// \param num   number of words looked for.
  /// \param len   maximum length of words looked for.
  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  shortest(const Automaton& aut,
           boost::optional<unsigned> num = {},
           boost::optional<unsigned> len = {})
  {
    detail::enumerater<Automaton> enumerater(aut);
    return enumerater(num, len);
  }


  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  /// \param len   maximum length of words looked for.
  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  enumerate(const Automaton& aut, unsigned len)
  {
    return shortest(aut, boost::none, len);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Num, typename Len>
      polynomial
      shortest(const automaton& aut,
               boost::optional<unsigned> num,
               boost::optional<unsigned> len)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a->context());
        return make_polynomial(ps, shortest(a, num, len));
      }
    }
  }
}
