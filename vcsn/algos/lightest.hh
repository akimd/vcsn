#pragma once

#include <algorithm>
#include <deque>
#include <fstream>

#include <boost/heap/binomial_heap.hpp>
#include <boost/optional.hpp>

#include <vcsn/core/name-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/labelset/word-polynomialset.hh>

namespace vcsn
{

  /*----------------------.
  | lightest(automaton).  |
  `----------------------*/

  namespace detail
  {

    /**
     * The lightest algorithm computes the paths between pre and post
     * with the smallest weight possible.
     *
     * This functor will construct the polynomial composed with each one
     * of the `num` smallest paths. This implementation uses a priority
     * queue that will order states by their weights (then labels).
     */
    template <typename Aut>
    class weighter
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<Aut>;

      using wordset_context_t = word_context_t<context_t>;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      /// Wordset.
      using labelset_t = labelset_t_of<polynomialset_t>;
      using word_t = word_t_of<automaton_t>;

      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      using datum_t = std::tuple<state_t, word_t, weight_t>;
      struct datum_less
      {
        /// Whether l < r (as this is a max heap).
        ///
        /// Compare values in this order:
        ///   - weight,
        ///   - label,
        ///   - whether one of them is post (post should be treated last),
        ///   - state number.
        /// Post is treated last in order to be sure that all the equivalent
        /// cases are treated before returning. When we finally reach post
        /// we know for sure that no smaller evaluation exists.
        bool operator()(const datum_t& r, const datum_t& l) const
        {
          if (weightset_t::less(std::get<2>(l), std::get<2>(r)))
            return true;
          else if (weightset_t::less(std::get<2>(r), std::get<2>(l)))
            return false;
          else if (labelset_t::less(std::get<1>(l), std::get<1>(r)))
            return true;
          else if (labelset_t::less(std::get<1>(r), std::get<1>(l)))
            return false;
          else if (std::get<0>(r) == automaton_t::element_type::post())
            return true;
          else
            return std::get<0>(l) != automaton_t::element_type::post()
                   && (std::get<0>(l) < std::get<0>(r));
        }
      };
      using queue_t =
        boost::heap::binomial_heap<datum_t,
                                   boost::heap::compare<datum_less>>;

      /// Prepare to compute an approximation of the behavior.
      ///
      /// \param aut   the automaton to approximate
      weighter(const automaton_t& aut)
        : aut_(aut)
      {}

      /// The approximated behavior of the automaton.
      /// \param num   number of words looked for.
      polynomial_t operator()(boost::optional<unsigned> num)
      {
        if (!num)
          num = 1;

        return lightest_(*num);
      }

    private:
      polynomial_t lightest_(unsigned num)
      {
        auto queue = queue_t{};
        queue.emplace(aut_->pre(), ls_.one(), ws_.one());

        // The approximated behavior: the first orders to post's past.
        polynomial_t res;
        while (!queue.empty() && num != res.size())
          {
            state_t s; word_t l; weight_t w;
            std::tie(s, l, w) = queue.top();

            queue.pop();

            /// Fuse equivalent cases, which might increase the first element's
            /// weight. Hence, restart loop with sorted queue.
            if (!queue.empty()
                && std::get<0>(queue.top()) == s
                && ls_.equal(std::get<1>(queue.top()), l))
              {
                while (!queue.empty()
                       && std::get<0>(queue.top()) == s
                       && ls_.equal(std::get<1>(queue.top()), l))
                  {
                    w = ws_.add(w, std::get<2>(queue.top()));
                    queue.pop();
                  }
                queue.emplace(s, l, w);
                continue;
              }

            if (s == aut_->post())
              ps_.add_here(res, std::move(l), std::move(w));

            for (const auto t: aut_->all_out(s))
              {
                auto dst = aut_->dst_of(t);
                auto nw = ws_.mul(w, aut_->weight_of(t));
                if (aut_->src_of(t) == aut_->pre() || dst == aut_->post())
                  queue.emplace(dst, l, std::move(nw));
                else
                  {
                    auto nl = ls_.mul(l, aut_->label_of(t));
                    queue.emplace(dst, std::move(nl), std::move(nw));
                  }
              }
          }

        return res;
      }

      /// Show the heap, for debugging.
      void show_heap_(const queue_t& q, std::ostream& os = std::cerr)
      {
        const char* sep = "";
        for (auto i = q.ordered_begin(), end = q.ordered_end();
             i != end; ++i)
          {
            os << sep;
            sep = " , ";
            aut_->print_state_name(std::get<0>(*i), os) << ":<";
            ws_.print(std::get<2>(*i), os);
            os << ">:";
            ls_.print(std::get<1>(*i), os);
          }
        os << '\n';
      }

      /// The automaton whose behavior to approximate.
      automaton_t aut_;
      const weightset_t& ws_ = *aut_->weightset();
      const polynomialset_t ps_ = make_word_polynomialset(aut_->context());
      const labelset_t& ls_ = *ps_.labelset();
    };
  }

  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  /// \param num   number of words looked for.
  template <typename Automaton>
  inline
  typename detail::weighter<Automaton>::polynomial_t
  lightest(const Automaton& aut, boost::optional<unsigned> num = {})
  {
    detail::weighter<Automaton> weighter(aut);
    return weighter(num);
  }


  /// The approximated behavior of an automaton.
  ///
  /// \param aut   the automaton whose behavior to approximate
  template <typename Automaton>
  inline
  typename detail::weighter<Automaton>::polynomial_t
  weigh(const Automaton& aut)
  {
    return lightest(aut, boost::none);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Num>
      polynomial
      lightest(const automaton& aut, boost::optional<unsigned> num)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a->context());
        return make_polynomial(ps, lightest(a, num));
      }
    }
  }
}
