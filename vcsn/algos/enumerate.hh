#ifndef VCSN_ALGOS_ENUMERATE_HH
# define VCSN_ALGOS_ENUMERATE_HH

# include <algorithm>
# include <iostream>
# include <list>
# include <map>
# include <queue>
# include <vector>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/wordset.hh>
# include <vcsn/weights/polynomialset.hh>

namespace vcsn
{


  /*-----------------------.
  | enumerate(automaton).  |
  `-----------------------*/

  namespace detail
  {
    /// The LAW from a LAL.
    template <typename Context>
    struct law_traits
    {};

    template <typename GenSet, typename WeightSet>
    struct law_traits<ctx::context<ctx::wordset<GenSet>, WeightSet>>
    {
      using in_type = ctx::context<ctx::wordset<GenSet>, WeightSet>;
      using type = ctx::context<ctx::wordset<GenSet>, WeightSet>;

      static type context(const in_type& ctx)
      {
        return ctx;
      }

      using polynomialset_t = polynomialset<type>;

      polynomialset_t
      word_polynomialset(const in_type& ctx)
      {
        return context(ctx);
      }
    };

    template <typename GenSet, typename WeightSet>
    struct law_traits<ctx::context<ctx::letterset<GenSet>, WeightSet>>
    {
      using in_type = ctx::context<ctx::letterset<GenSet>, WeightSet>;
      using type = ctx::context<ctx::wordset<GenSet>, WeightSet>;

      static type context(const in_type& ctx)
      {
        return {*ctx.labelset()->genset(), *ctx.weightset()};
      }

      using polynomialset_t = polynomialset<type>;

      polynomialset_t
      word_polynomialset(const in_type& ctx)
      {
        return context(ctx);
      }
    };

    template <typename Context>
    auto
    make_word_polynomialset(const Context& ctx)
      -> typename law_traits<Context>::polynomialset_t
    {
      return law_traits<Context>::context(ctx);
    }


    template <typename Aut>
    class enumerater
    {
    public:
      using automaton_t = Aut;
      using context_t = typename Aut::context_t;
      static_assert(context_t::is_lal,
                    "requires labels_are_letters");

      using labelset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;
      using wordset_context_t = typename law_traits<context_t>::type;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;
      using state_t = typename automaton_t::state_t;
      using genset_t = typename automaton_t::labelset_t::genset_t;
      using word_t = typename genset_t::word_t;

      /// Same as polynomial_t::value_type.
      using monomial_t = std::pair<word_t, weight_t>;
      using queue_t = std::list<std::pair<state_t, monomial_t>>;

      enumerater(const automaton_t& aut)
        : aut_(aut)
      {
        ps_.set_weight(past_[aut_.pre()],
                       ls_.genset()->empty_word(),
                       aut_.weightset()->one());
      }

      /// The weighted accepted word with length at most \a max.
      polynomial_t operator()(size_t max)
      {
        queue_t queue;

        // FIXME: check piecewise_construct.
        queue.emplace_back(aut_.pre(),
                           monomial_t{ls_.genset()->empty_word(),
                               aut_.weightset()->one()});

        // We match words that include the initial and final special
        // character.
        max += 2;
        for (size_t i = 0; i < max && not queue.empty(); ++i)
          propagate_(queue);

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: past_[aut_.post()])
          ps_.add_weight(res,
                         ls_.genset()->undelimit(m.first), m.second);
        return res;
      }

      /// The shortest accepted weighted word, or throw an exception.
      // FIXME: code duplication.
      monomial_t operator()()
      {
        queue_t queue;

        // FIXME: check piecewise_construct.
        queue.emplace_back(aut_.pre(),
                           monomial_t{ls_.genset()->empty_word(),
                               aut_.weightset()->one()});

        while (past_[aut_.post()].empty() && !queue.empty())
          propagate_(queue);

        if (past_[aut_.post()].empty())
          throw std::domain_error("shortest: the automaton is useless");
        else
          {
            // There is no "front()" method.
            const auto& res = *past_[aut_.post()].begin();
            return {ls_.genset()->undelimit(res.first), res.second};
          }
      }

    private:
      /// Process once all the states of \a q1.
      /// Save into q1 the new states to visit.
      void propagate_(queue_t& q1)
      {
        queue_t q2;
        while (!q1.empty())
          {
            state_t s;
            monomial_t m;
            tie(s, m) = std::move(q1.front());
            q1.pop_front();
            for (const auto t: aut_.all_out(s))
              {
                // FIXME: monomial mul.
                monomial_t n(ls_.concat(m.first, aut_.label_of(t)),
                             ws_.mul(m.second, aut_.weight_of(t)));
                ps_.add_weight(past_[aut_.dst_of(t)], n);
                q2.emplace_back(aut_.dst_of(t), n);
              }
          }
        q1.swap(q2);
      }

      const automaton_t& aut_;
      const labelset_t& ls_ = *aut_.labelset();
      const weightset_t& ws_ = *aut_.weightset();
      const polynomialset_t ps_
        = {law_traits<context_t>::context(aut_.context())};
      /// For each state, the first orders of its past.
      std::map<state_t, polynomial_t> past_;
    };
  }

  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  enumerate(const Automaton& aut, size_t max)
  {
    detail::enumerater<Automaton> enumerate(aut);
    return enumerate(max);
  }

  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::monomial_t
  shortest(const Automaton& aut)
  {
    detail::enumerater<Automaton> shortest(aut);
    return shortest();
  }


  namespace dyn
  {
    namespace detail
    {

      /*-----------------.
      | dyn::enumerate.  |
      `-----------------*/

      // FIXME: We need dyn::polynomial.
      template <typename Aut>
      polynomial
      enumerate(const automaton& aut, size_t max)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a.context());
        return make_polynomial(ps, enumerate(a, max));
      }

      REGISTER_DECLARE
      (enumerate,
       (const automaton& aut, size_t max) -> polynomial);


      /*----------------.
      | dyn::shortest.  |
      `----------------*/

      // FIXME: We need dyn::label.
      template <typename Aut>
      std::string
      shortest(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a.context());
        return ps.format(shortest(a));
      }

      REGISTER_DECLARE(shortest,
                       (const automaton& aut) -> std::string);
    }
  }
}

#endif // !VCSN_ALGOS_ENUMERATE_HH
