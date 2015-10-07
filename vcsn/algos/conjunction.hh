#pragma once

#include <iostream>
#include <map>
#include <utility>

#include <vcsn/algos/insplit.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/core/lazy-tuple-automaton.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/expression.hh> // dyn::make_expression
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/misc/tuple.hh> // tuple_element_t, cross_tuple
#include <vcsn/misc/zip-maps.hh>

namespace vcsn
{
  namespace detail
  {
    /*---------------------------------.
    | product_automaton_impl<Aut...>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) product.
    template <Automaton Aut, Automaton... Auts>
    class product_automaton_impl
      : public lazy_tuple_automaton<product_automaton_impl<Aut, Auts...>, false, Aut, Auts...>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "product: requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using tuple_automaton_t = tuple_automaton<automaton_t, Auts...>;
      using self_t = product_automaton_impl;
      using super_t = lazy_tuple_automaton<self_t, false, Aut, Auts...>;

    public:
      using state_name_t = typename super_t::state_name_t;
      using state_t = typename super_t::state_t;

      template <Automaton A>
      using transition_map_t = typename super_t::template transition_map_t<A>;

      template <size_t... I>
      using seq
        = typename tuple_automaton_t::element_type::template seq<I...>;

      using super_t::ws_;
      using super_t::transition_maps_;

      static symbol sname()
      {
        static symbol res("product_automaton"
                          + super_t::sname_());
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "product_automaton";
        return aut_->print_set_(o, fmt);
      }

      /// The context of the result.
      using context_t = context_t_of<Aut>;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// The type of input automata.
      using automata_t = std::tuple<Auts...>;

      /// The type of the Ith input automaton, unqualified.
      template <size_t I>
      using input_automaton_t = base_t<tuple_element_t<I, automata_t>>;

      using super_t::aut_;

      /// Build a product automaton.
      /// \param aut   the automaton to build.
      /// \param auts  the input automata.
      product_automaton_impl(Aut aut, const Auts&... auts)
        : super_t{aut, auts...}
      {}

      /// Compute the (accessible part of the) conjunction.
      void conjunction(bool lazy = false)
      {
        initialize_conjunction();

        if (!lazy)
          while (!aut_->todo_.empty())
            {
              const auto& p = aut_->todo_.front();
              this->complete_(std::get<1>(p));
              aut_->todo_.pop_front();
            }
      }

      /// Compute the (accessible part of the) shuffle product.
      void shuffle()
      {
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            add_shuffle_transitions<false>(std::get<1>(p), std::get<0>(p));
            aut_->todo_.pop_front();
          }
      }

      /// Compute the (accessible part of the) infiltration product.
      void infiltration()
      {
        // Variadic infiltration is not trivial to implement, it's not
        // just conjunction and shuffle in series.  For instance, consider
        // three automata:
        //
        //           <x>a
        // x = -> 0 ------> 1 ->
        //
        // and likewise for y and z.  Let's use `&:` to denote
        // infiltration.  In (x &: y) there is a transition ((0,0),
        // <xy>a, (1,1)) coming from the conjunction-like transitions.
        //
        // Therefore in (x &: y) &: z there is a transition ((0,0),0),
        // <xy>a, (1,1), 0) by virtue of the shuffle-like transitions.
        //
        // This kind of transition that mixes conjunction and shuffle
        // would never appear in a naive implementation with only
        // conjunction and shuffle transitions, but no combinations.
        require(sizeof...(Auts) == 2,
                "infiltration: variadic product does not work");

        // Infiltrate is a mix of conjunction and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for conjunction:
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            // Infiltrate is a mix of conjunction and shuffle operations.
            //
            // Conjunction transitions must be added before shuffle ones:
            // this way "conjunction" can use "new_transition" only, which
            // is faster than "add_transition".
            add_conjunction_transitions(std::get<1>(p), std::get<0>(p));
            add_shuffle_transitions<true>(std::get<1>(p), std::get<0>(p));

            aut_->todo_.pop_front();
          }
      }

      /// Tell lazy_tuple_automaton how to add the transitions to a state
      void add_transitions(const state_t src,
                           const state_name_t& psrc)
      {
        add_conjunction_transitions(src, psrc);
      }

    private:
      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the conjunction algorithm.
      void initialize_conjunction()
      {
        aut_->todo_.emplace_back(aut_->pre_(), aut_->pre());
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle()
      {
        // Make the result automaton initial states: same as the
        // conjunction of pre: synchronized transitions on $.
        add_conjunction_transitions(aut_->pre(), aut_->pre_());
      }


      using super_t::out_;
      using super_t::state;
      /// Add transitions to the result automaton, starting from the
      /// given result input state, which must correspond to the given
      /// pair of input state automata.  Update the worklist with the
      /// needed source-state pairs.
      void add_conjunction_transitions(const state_t src,
                                       const state_name_t& psrc)
      {
        for (auto t: zip_map_tuple(out_(psrc)))
          // These are always new transitions: first because the
          // source state is visited for the first time, and second
          // because the couple (left destination, label) is unique,
          // and so is (right destination, label).
          if (!aut_->labelset()->is_one(t.first))
            detail::cross_tuple
              ([this,src,&t]
               (const typename transition_map_t<Auts>::transition&... ts)
               {
                 this->new_transition(src, state(ts.dst...),
                                      t.first,
                                      ws_.mul(ts.weight()...));
               },
               t.second);
        add_one_transitions_(src, psrc, aut_->indices);
      }

      /// Add the spontaneous transitions leaving state \a src, if it
      /// is relevant (i.e. only for the labelsets that have one).
      template <std::size_t... I>
      void
      add_one_transitions_(const state_t src, const state_name_t& psrc,
                           seq<I...>)
      {
        using swallow = int[];
        (void) swallow
        {
          (maybe_add_one_transitions_<I>(*(std::get<I>(aut_->auts_)->labelset()),
                                         src, psrc), 0)...
        };
      }

      /// In the case where the labelset doesn't have one, do nothing
      template <std::size_t I, typename L>
      std::enable_if_t<!L::has_one(), void>
      maybe_add_one_transitions_(const L&, const state_t, const state_name_t&)
      {}

      /// If the labelset has one, add the relevant
      /// spontaneous-transitions leaving the state
      template <std::size_t I, typename L>
      std::enable_if_t<L::has_one(), void>
      maybe_add_one_transitions_(const L& ls, const state_t src,
                                 const state_name_t& psrc)
      {
        if (!has_one_in(psrc, I + 1, aut_->indices)
            && !has_only_one_out(psrc, I, aut_->indices))
          {
            // one is guaranteed to be first.
            const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
            if (!tmap.empty() && ls.is_one(tmap.begin()->first))
              for (auto t : tmap.begin()->second)
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = t.dst;
                  this->new_transition(src, state(pdst), ls.one(), t.weight());
                }
          }
      }

      /// Check if all the tapes after the Ith have only incoming
      /// spontaneous transitions.
      template <std::size_t... I>
      bool has_one_in(const state_name_t& psrc, std::size_t i,
                      seq<I...>) const
      {
        bool has_ones[] = { is_spontaneous_in(std::get<I>(aut_->auts_),
                                             std::get<I>(psrc))... };
        for (; i < sizeof...(Auts); ++i)
          if (has_ones[i])
            return true;
        return false;
      }

      /// Check if all the tapes before the Ith have only outgoing
      /// spontaneous transitions.
      template <std::size_t... I>
      bool has_only_one_out(const state_name_t& psrc, std::size_t i,
                            seq<I...>)
      {
        bool has_ones[] = { has_only_ones_out<I>(psrc)... };
        for (size_t j = 0; j < i; ++j)
          if (has_ones[j])
            return true;
        return false;
      }

      /// Check if the transition is spontaneous (in the case of a
      /// labelset with one).
      template <Automaton Aut_>
      std::enable_if_t<labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_& aut, transition_t_of<Aut_> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      /// Same as above, but for labelsets without one, so it's always
      /// false.
      template <Automaton Aut_>
      constexpr std::enable_if_t<!labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_&, transition_t_of<Aut_>) const
      {
        return false;
      }

      /// Check if the state has only incoming spontaneous
      /// transitions.  As it is in the case of the one-free labelset,
      /// it's always false.
      template <Automaton Aut_>
      constexpr std::enable_if_t<!labelset_t_of<Aut_>::has_one(), bool>
      is_spontaneous_in(const Aut_&,
                       state_t_of<Aut_>) const
      {
        return false;
      }

      /// Whether the state has only incoming spontaneous transitions.
      /// The automaton has been insplit, so either all incoming transitions
      /// are proper, or all transitions are spontaneous (including the first
      /// one).
      template <Automaton Aut_>
      std::enable_if_t<labelset_t_of<Aut_>::has_one(), bool>
      is_spontaneous_in(const Aut_& rhs, state_t_of<Aut_> rst) const
      {
        auto rin = all_in(rhs, rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr);
      }

      /// Whether the Ith state of \a psrc in the Ith input automaton
      /// has no non-spontaneous outgoing transitions.
      template <size_t I>
      bool
      has_only_ones_out(const state_name_t& psrc)
      {
        const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        auto s = tmap.size();
        if (s == 0)
          return true;
        else if (2 <= s)
          return false;
        else
          return std::get<I>(aut_->auts_)->labelset()->is_one(tmap.begin()->first);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given tuple of input state automata.  Update the worklist
      /// with the needed source-state pairs.
      template <bool Infiltration = false>
      void add_shuffle_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        weight_t final
          = add_shuffle_transitions_<Infiltration>(src, psrc, aut_->indices);
        aut_->set_final(src, final);
      }

      /// Let all automata advance one after the other, and add the
      /// corresponding transitions in the output.
      ///
      /// Return the product of the final states.
      template <bool Infiltration, size_t... I>
      weight_t add_shuffle_transitions_(const state_t src,
                                        const state_name_t& psrc,
                                        seq<I...>)
      {
        weight_t res = ws_.one();
        using swallow = int[];
        (void) swallow
        {
          (res = ws_.mul(res,
                         add_shuffle_transitions_<Infiltration, I>(src, psrc)),
           0)...
        };
        return res;
      }

      /// Let Ith automaton advance, and add the corresponding
      /// transitions in the output.
      ///
      /// If we reach a final state, return the corresponding final
      /// weight (zero otherwise).
      ///
      /// \tparam Infiltration
      ///    whether we are called after add_conjunction_transitions.
      /// \tparam I
      ///    the tape on which to perform a transition.
      template <bool Infiltration, size_t I>
      weight_t
      add_shuffle_transitions_(const state_t src,
                               const state_name_t& psrc)
      {
        // Whether is a final state.
        weight_t res = ws_.zero();

        auto& ts = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        for (auto t: ts)
          if (std::get<I>(aut_->auts_)->labelset()->is_special(t.first))
            res = t.second.front().weight();
          else
            // The src state is visited for the first time, so all
            // these transitions are new.  *Except* in the case where
            // we have a loop on some tapes.
            //
            // If add_conjunction_transitions was called before (in the
            // case of infiltration), there may even exist such a
            // transition in the first loop.
            //
            // To trigger the later case, try the self-infiltration on
            // derived_term('a*a').
            for (auto d: t.second)
              {
                auto pdst = psrc;
                std::get<I>(pdst) = d.dst;
                if (Infiltration
                    || std::get<I>(psrc) == d.dst)
                  this->add_transition(src, state(pdst), t.first, d.weight());
                else
                  this->new_transition(src, state(pdst), t.first, d.weight());
              }
        return res;
      }
    };
  }

  /// A product automaton as a shared pointer.
  template <Automaton Aut, Automaton... Auts>
  using product_automaton
    = std::shared_ptr<detail::product_automaton_impl<Aut, Auts...>>;

  template <Automaton Aut, Automaton... Auts>
  auto
  make_product_automaton(Aut aut, const Auts&... auts)
    -> product_automaton<Aut, Auts...>
  {
    using res_t = product_automaton<Aut, Auts...>;
    return make_shared_ptr<res_t>(aut, auts...);
  }


  /*-----------------------------.
  | conjunction(automaton...).   |
  `-----------------------------*/

  /// Build the (accessible part of the) conjunction.
  template <Automaton... Auts>
  auto
  conjunction(const Auts&... as)
    -> tuple_automaton<decltype(meet_automata(as...)),
                       Auts...>
  {
    auto res = make_product_automaton(meet_automata(as...),
                                      as...);
    res->conjunction();
    return res->strip();
  }

  /// Build the (accessible part of the) conjunction, on-the-fly.
  template <Automaton... Auts>
  auto
  conjunction_lazy(const Auts&... as)
    -> product_automaton<decltype(meet_automata(as...)),
                         Auts...>
  {
    auto res = make_product_automaton(meet_automata(as...),
                                      as...);
    res->conjunction(true);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      template <std::size_t I, Automaton Aut>
      auto
      do_insplit(Aut& aut)
        -> std::enable_if_t<labelset_t_of<Aut>::has_one() && I != 0,
                             decltype(insplit(aut))>
      {
        return insplit(aut);
      }

      template <std::size_t I, Automaton Aut>
      std::enable_if_t<!labelset_t_of<Aut>::has_one() || I == 0, Aut&>
      do_insplit(Aut& aut)
      {
        return aut;
      }

      /// Bridge helper.
      template <typename Auts, size_t... I>
      automaton
      conjunction_(const std::vector<automaton>& as, bool lazy,
                   vcsn::detail::index_sequence<I...>)
      {
        if (lazy)
          return make_automaton
            (vcsn::conjunction_lazy
             (do_insplit<I>(as[I]->as<tuple_element_t<I, Auts>>())...));
        else
          return make_automaton
            (vcsn::conjunction
             (do_insplit<I>(as[I]->as<tuple_element_t<I, Auts>>())...));
      }

      /// Bridge (conjunction).
      template <typename Auts, typename Bool>
      automaton
      conjunction(const std::vector<automaton>& as, bool lazy)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return conjunction_<Auts>(as, lazy, indices);
      }
    }
  }


  /*------------------------.
  | shuffle(automaton...).  |
  `------------------------*/

  /// The (accessible part of the) shuffle product.
  template <Automaton... Auts>
  auto
  shuffle(const Auts&... as)
    -> tuple_automaton<decltype(join_automata(as...)),
                       Auts...>
  {
    auto res = make_product_automaton(join_automata(as...), as...);
    res->shuffle();
    return res->strip();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename Auts, size_t... I>
      automaton
      shuffle_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::shuffle(as[I]->as<tuple_element_t<I, Auts>>()...);
        return make_automaton(res);
      }

      /// Bridge (shuffle).
      template <typename Auts>
      automaton
      shuffle(const std::vector<automaton>& as)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return shuffle_<Auts>(as, indices);
      }
    }
  }


  /*-----------------------------------.
  | shuffle(expression, expression).   |
  `-----------------------------------*/

  /// Shuffle product of expressions.
  template <typename ValueSet>
  typename ValueSet::value_t
  shuffle(const ValueSet& vs,
          const typename ValueSet::value_t& lhs,
          const typename ValueSet::value_t& rhs)
  {
    return vs.shuffle(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (shuffle).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      shuffle_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return make_expression(std::get<0>(join_elts),
                               ::vcsn::shuffle(std::get<0>(join_elts),
                                               std::get<1>(join_elts),
                                               std::get<2>(join_elts)));
      }
    }
  }


  /*-----------------------------.
  | infiltration(automaton...).  |
  `-----------------------------*/

  /// The (accessible part of the) infiltration product.
  template <Automaton A1, Automaton A2>
  auto
  infiltration(const A1& a1, const A2& a2)
    -> tuple_automaton<decltype(join_automata(a1, a2)),
                       A1, A2>
  {
    auto res = make_product_automaton(join_automata(a1, a2), a1, a2);
    res->infiltration();
    return res->strip();
  }

  /// The (accessible part of the) infiltration product.
  template <Automaton A1, Automaton A2, Automaton A3, Automaton... Auts>
  auto
  infiltration(const A1& a1, const A2& a2, const A3& a3, const Auts&... as)
    -> decltype(infiltration(infiltration(a1, a2), a3, as...))
  {
    return infiltration(infiltration(a1, a2), a3, as...);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename Auts, size_t... I>
      automaton
      infiltration_(const std::vector<automaton>& as,
                    vcsn::detail::index_sequence<I...>)
      {
        auto res
          = vcsn::infiltration(as[I]->as<tuple_element_t<I, Auts>>()...);
        return make_automaton(res);
      }

      /// Bridge (infiltration).
      template <typename Auts>
      automaton
      infiltration(const std::vector<automaton>& as)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return infiltration_<Auts>(as, indices);
      }
    }
  }

  /*----------------------------------------.
  | infiltration(expression, expression).   |
  `----------------------------------------*/

  /// Infiltration product of expressions.
  template <typename ValueSet>
  typename ValueSet::value_t
  infiltration(const ValueSet& vs,
               const typename ValueSet::value_t& lhs,
               const typename ValueSet::value_t& rhs)
  {
    return vs.infiltration(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltration).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      infiltration_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return make_expression(std::get<0>(join_elts),
                               ::vcsn::infiltration(std::get<0>(join_elts),
                                                    std::get<1>(join_elts),
                                                    std::get<2>(join_elts)));
      }
    }
  }


  /*-----------------------------.
  | conjunction(automaton, n).   |
  `-----------------------------*/

  template <Automaton Aut>
  auto
  conjunction(const Aut& aut, unsigned n)
    -> fresh_automaton_t_of<Aut>
  {
    auto res = make_fresh_automaton(aut);
    {
      // automatonset::one().
      auto s = res->new_state();
      res->set_initial(s);
      res->set_final(s);
      for (auto l: res->context().labelset()->generators())
        res->new_transition(s, s, l);
    }

    if (n)
      {
        // FIXME: for 1, we should return the accessible part only.
        static bool iterative = getenv("VCSN_ITERATIVE");
        if (iterative)
          for (size_t i = 0; i < n; ++i)
            res = strip(conjunction(res, aut));
        else
          {
            auto power = strip(aut);
            while (true)
              {
                if (n % 2)
                  res = strip(conjunction(res, power));
                n /= 2;
                if (!n)
                  break;
                power = strip(conjunction(power, power));
              }
          }
      }

    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <Automaton Aut, typename Unsigned>
      automaton
      conjunction_repeated(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::conjunction(a, n));
      }
    }
  }

  /*-----------------------------.
  | conjunction(value, value).   |
  `-----------------------------*/

  /// Intersection/Hadamard product of expressions/polynomials.
  template <typename ValueSet>
  typename ValueSet::value_t
  conjunction(const ValueSet& rs,
              const typename ValueSet::value_t& lhs,
              const typename ValueSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rhs);
  }

  /*---------------------------------------.
  | conjunction(expression, expression).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      conjunction_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return make_expression(std::get<0>(join_elts),
                               ::vcsn::conjunction(std::get<0>(join_elts),
                                                   std::get<1>(join_elts),
                                                   std::get<2>(join_elts)));
      }
    }
  }

  /*---------------------------------------.
  | conjunction(polynomial, polynomial).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      conjunction_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return make_polynomial(std::get<0>(join_elts),
                               ::vcsn::conjunction(std::get<0>(join_elts),
                                                   std::get<1>(join_elts),
                                                   std::get<2>(join_elts)));
      }
    }
  }
}
