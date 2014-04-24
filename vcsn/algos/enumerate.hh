#ifndef VCSN_ALGOS_ENUMERATE_HH
# define VCSN_ALGOS_ENUMERATE_HH

# include <algorithm>
# include <iostream>
# include <list>
# include <map>
# include <queue>
# include <vector>

# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/nullableset.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/labelset/wordset.hh>
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{


  /*-----------------------.
  | enumerate(automaton).  |
  `-----------------------*/

  namespace detail
  {
    /// The LAW from a LAL.
    template <typename ValueSet>
    struct law_traits
    {};

    template <typename LabelSet>
    using law_t = typename law_traits<LabelSet>::type;

    template <typename LabelSet>
    inline law_t<LabelSet>
    make_wordset(const LabelSet& ls);

    // Converting labelsets.
    template <>
    struct law_traits<oneset>
    {
      using type = oneset;
      static type value(oneset)
      {
        return {};
      }
    };

    template <typename GenSet>
    struct law_traits<letterset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const letterset<GenSet>& ls)
      {
        return ls.genset();
      }
    };

    template <typename GenSet>
    struct law_traits<wordset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const wordset<GenSet>& ls)
      {
        return ls;
      }
    };

    template <typename LabelSet>
    struct law_traits<nullableset<LabelSet>>
    {
      using type = law_t<LabelSet>;
      static type value(const nullableset<LabelSet>& ls)
      {
        return make_wordset(*ls.labelset());
      }
    };

    template <typename... LabelSets>
    struct law_traits<tupleset<LabelSets...>>
    {
      using labelset_t = tupleset<LabelSets...>;
      using type = tupleset<law_t<LabelSets>...>;

      template <std::size_t... I>
      static type value(const labelset_t& ls, detail::index_sequence<I...>)
      {
        return {make_wordset(ls.template set<I>())...};
      }

      static type value(const labelset_t& ls)
      {
        return value(ls, detail::make_index_sequence<sizeof...(LabelSets)>{});
      }
    };

    template <typename LabelSet>
    inline law_t<LabelSet>
    make_wordset(const LabelSet& ls)
    {
      return law_traits<LabelSet>::value(ls);
    };

    template <typename Ctx>
    using word_context_t
      = context<law_t<typename Ctx::labelset_t>, typename Ctx::weightset_t>;

    template <typename LabelSet, typename WeightSet>
    inline word_context_t<context<LabelSet, WeightSet>>
    make_word_context(const context<LabelSet, WeightSet>& ctx)
    {
      return {make_wordset(*ctx.labelset()), *ctx.weightset()};
    }

    template <typename Ctx>
    using word_polynomialset_t = polynomialset<word_context_t<Ctx>>;

    template <typename Ctx>
    inline auto
    make_word_polynomialset(const Ctx& ctx)
      -> word_polynomialset_t<Ctx>
    {
      return make_word_context(ctx);
    }


    template <typename Aut>
    class enumerater
    {
    public:
      using automaton_t = Aut;
      using context_t = typename Aut::context_t;
      static_assert(context_t::labelset_t::is_free(),
                    "requires labels_are_letters");

      using labelset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;
      using wordset_context_t = word_context_t<context_t>;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;
      using state_t = typename automaton_t::state_t;
      using genset_t = typename automaton_t::labelset_t::genset_t;
      using word_t = typename labelset_t::word_t;

      /// Same as polynomial_t::value_type.
      using monomial_t = std::pair<word_t, weight_t>;
      using queue_t = std::list<std::pair<state_t, monomial_t>>;

      enumerater(const automaton_t& aut)
        : aut_(aut)
      {
        past_[aut_.pre()] = ps_.one();
      }

      /// The weighted accepted word with length at most \a max.
      polynomial_t enumerate(unsigned max)
      {
        queue_t queue;
        queue.emplace_back(aut_.pre(), ps_.monomial_one());

        // We match words that include the initial and final special
        // characters.
        max += 2;
        for (size_t i = 0; i < max && !queue.empty(); ++i)
          propagate_(queue);

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: past_[aut_.post()])
          ps_.add_weight(res,
                         ls_.undelimit(m.first), m.second);
        return res;
      }

      /// The shortest accepted weighted words, or throw an exception.
      // FIXME: code duplication.
      polynomial_t shortest(unsigned num)
      {
        queue_t queue;
        queue.emplace_back(aut_.pre(), ps_.monomial_one());

        while (past_[aut_.post()].size() < num && !queue.empty())
          propagate_(queue);

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: past_[aut_.post()])
          {
            ps_.add_weight(res,
                           ls_.undelimit(m.first), m.second);
            if (--num == 0)
              break;
          }
        return res;
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
      const weightset_t& ws_ = *aut_.weightset();
      const polynomialset_t ps_ = make_word_polynomialset(aut_.context());
      const typename polynomialset_t::labelset_t& ls_ = *ps_.labelset();
      /// For each state, the first orders of its past.
      std::map<state_t, polynomial_t> past_;
    };
  }

  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  enumerate(const Automaton& aut, unsigned max)
  {
    detail::enumerater<Automaton> enumerater(aut);
    return enumerater.enumerate(max);
  }

  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  shortest(const Automaton& aut, unsigned num)
  {
    detail::enumerater<Automaton> enumerater(aut);
    return enumerater.shortest(num);
  }


  namespace dyn
  {
    namespace detail
    {

      /*-----------------.
      | dyn::enumerate.  |
      `-----------------*/

      template <typename Aut, typename Unsigned>
      polynomial
      enumerate(const automaton& aut, unsigned max)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a.context());
        return make_polynomial(ps, enumerate(a, max));
      }

      REGISTER_DECLARE
      (enumerate,
       (const automaton& aut, unsigned max) -> polynomial);


      /*----------------.
      | dyn::shortest.  |
      `----------------*/

      template <typename Aut, typename Unsigned>
      polynomial
      shortest(const automaton& aut, unsigned num)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a.context());
        return make_polynomial(ps, shortest(a, num));
      }

      REGISTER_DECLARE(shortest,
                        (const automaton& aut, unsigned num) -> polynomial);
    }
  }
}

#endif // !VCSN_ALGOS_ENUMERATE_HH
