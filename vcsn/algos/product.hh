#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

# include <deque>
# include <iostream>
# include <map>
# include <utility>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/copy.hh>

namespace vcsn
{

  namespace detail
  {

    /*----------.
    | product.  |
    `----------*/

    /// Build the (accessible part of the) product.
    template <typename A, typename B>
    class producter
    {
      static_assert(A::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(B::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(A::context_t::weightset_t::is_commutative_semiring(),
                    "not a commutative semiring");

      using automaton_t = A;

      using state_t = typename automaton_t::state_t;
      using pair_t = std::pair<typename A::state_t, typename B::state_t>;
      using label_t = typename automaton_t::label_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      /// Input automata, supplied at construction time.
      const A& laut_;
      const B& raut_;

      /// Map (left-state, right-state) -> product-state.
      using map = std::map<pair_t, state_t>;
      map pmap_;

      /// Worklist of (left-state, right-state).
      std::deque<pair_t> todo_;

      /// Add the pre and post states in the result automaton.  This
      /// is needed for all three algorithms here.
      void initialize()
      {
        pair_t ppre(laut_.pre(), raut_.pre());
        pair_t ppost(laut_.post(), raut_.post());
        pmap_[ppre] = res_.pre();
        pmap_[ppost] = res_.post();
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_product()
      {
        initialize();
        todo_.emplace_back(pair_t(laut_.pre(), raut_.pre()));
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle(const weightset_t& ws)
      {
        initialize();

        /// Make the result automaton initial states:
        for (auto lt : laut_.initial_transitions())
          for (auto rt : raut_.initial_transitions())
            {
              auto lsrc = laut_.dst_of(lt);
              auto rsrc = raut_.dst_of(rt);
              pair_t pair(lsrc, rsrc);
              state_t init = res_.new_state();
              res_.add_initial(init,
                               ws.mul(laut_.weight_of(lt),
                                      raut_.weight_of(rt)));
              pmap_[pair] = init;
              todo_.emplace_back(pair);
            }
      }

      /// The state in the product corresponding to a pair of states
      /// of operands.
      /// 
      /// Add the given two source-automaton states to the worklist
      /// for the given result automaton if they aren't already there,
      /// updating the map; in any case return.
      state_t state(typename A::state_t lst, typename B::state_t rst)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        pair_t pdst(lst, rst);
        auto iter = pmap_.find(pdst);
        state_t dst;
        if (iter == pmap_.end())
          {
            dst = res_.new_state();
            pmap_[pdst] = dst;
            todo_.emplace_back(pdst);
          }
        else
          dst = iter->second;
        return dst;
      }

      /// Add a transition in the result from destination states in operands.
      void
      add_transition(state_t src,
                     typename A::state_t ldst, typename B::state_t rdst,
                     const label_t& label, const weight_t& weight)
      {
        res_.add_transition(src, state(ldst, rdst), label, weight);
      }


      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correstpond to the
      /// givenpair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_product_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        for (auto lt : laut_.all_out(psrc.first))
          {
            auto label = laut_.label_of(lt);
            auto lweight = laut_.weight_of(lt);
            auto ldst = laut_.dst_of(lt);

            for (auto rt : raut_.out(psrc.second, label))
              add_transition(src, ldst, raut_.dst_of(rt),
                             label, ws.mul(lweight, raut_.weight_of(rt)));
          }
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correstpond to the
      /// givenpair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_shuffle_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        state_t lsrc = psrc.first;
        state_t rsrc = psrc.second;
        if (laut_.is_final(lsrc) && raut_.is_final(rsrc))
          res_.set_final(src,
                         ws.mul(laut_.get_final_weight(lsrc),
                                raut_.get_final_weight(rsrc)));

        for (auto lt : laut_.out(lsrc))
          add_transition(src, laut_.dst_of(lt), rsrc,
                         laut_.label_of(lt), laut_.weight_of(lt));
        for (auto rt : raut_.out(rsrc))
          add_transition(src, lsrc, raut_.dst_of(rt),
                         raut_.label_of(rt), raut_.weight_of(rt));
      }

    public:
      producter(const A& laut, const B& raut)
        : laut_(laut), raut_(raut), res_(laut.context())
      {}

      /// Reset the attributes before a new product.
      void clear()
      {
        pmap_.clear();
        todo_.clear();
      }

      /// The (accessible part of the) product of \a laut_ and \a raut_.
      automaton_t product()
      {
        auto ctx = intersection(laut_.context(), raut_.context());
        const auto& ws = *ctx.weightset();
        res_ = std::move(automaton_t(ctx));

        initialize_product();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_product_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }

      /// The (accessible part of the) shuffle of \a laut_ and \a raut_.
      automaton_t shuffle()
      {
        auto ctx = get_union(laut_.context(), raut_.context());
        const auto& ws = *ctx.weightset();
        res_ = automaton_t(ctx);

        initialize_shuffle(ws);

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_shuffle_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }

      /// The (accessible part of the) infiltration of \a laut_ and \a raut_.
      automaton_t infiltration()
      {
        auto ctx = get_union(laut_.context(), raut_.context());
        const auto& ws = *ctx.weightset();
        res_ = automaton_t(ctx);

        // Infiltrate is a mix of product and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for product:
        initialize_shuffle(ws);

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            // Infiltrate is a mix of product and shuffle operations.
            add_product_transitions(ws, src, psrc);
            add_shuffle_transitions(ws, src, psrc);
          }

        return std::move(res_);
      }

      /// A map from product states to pair of original states.
      using origins_t = std::map<state_t, pair_t>;
      origins_t
      origins() const
      {
        std::map<state_t, pair_t> res;
        for (const auto& p: pmap_)
          res.emplace(p.second, p.first);
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins." << std::endl
          << "    node [shape = box, style = rounded]" << std::endl;
        for (auto p: orig)
          if (p.first != automaton_t::pre() && p.first != automaton_t::post())
            o << "    " << p.first - 2
              << " [label = \""
              << p.second.first - 2
              << ","
              << p.second.second - 2
              << "\"]"
              << std::endl;
        o << "*/" << std::endl;
        return o;
      }

    private:
      /// The computed product.
      automaton_t res_;
    };
  }

  /*---------.
  | product. |
  `---------*/

  /// Build the (accessible part of the) product.
  template <typename A, typename B>
  A
  product(const A& laut, const B& raut)
  {
    detail::producter<A, B> product(laut, raut);
    auto res = product.product();
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }


  /*----------.
  | shuffle.  |
  `----------*/

  /// Build the (accessible part of the) shuffle.
  template <typename A, typename B>
  A
  shuffle(const A& laut, const B& raut)
  {
    detail::producter<A, B> product(laut, raut);
    auto res = product.shuffle();
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }


  /*---------------.
  | infiltration.  |
  `---------------*/

  /// Build the (accessible part of the) infiltration.
  template <typename A, typename B>
  A
  infiltration(const A& laut, const B& raut)
  {
    detail::producter<A, B> product(laut, raut);
    auto res = product.infiltration();
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }


  /*--------.
  | power.  |
  `--------*/

  template <typename Aut>
  Aut
  power(const Aut& aut, unsigned n)
  {
    Aut res(aut.context());
    {
      // automatonset::one().
      auto s = res.new_state();
      res.set_initial(s);
      res.set_final(s);
      for (auto l: *res.context().labelset())
        res.set_transition(s, s, l);
    }

    if (n)
      {
        static bool iterative = getenv("VCSN_ITERATIVE");
        if (iterative)
          for (size_t i = 0; i < n; ++i)
            res = std::move(product(res, aut));
        else
          {
            Aut power = copy(aut);
            while (true)
              {
                if (n % 2)
                  res = std::move(product(res, power));
                n /= 2;
                if (!n)
                  break;
                power = std::move(product(power, power));
              }
          }
      }

    return res;
  }


  namespace dyn
  {
    namespace detail
    {

      /*---------------.
      | dyn::product.  |
      `---------------*/

      template <typename Lhs, typename Rhs>
      automaton
      product(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(product(l, r));
      }

      REGISTER_DECLARE2(product,
                        (const automaton&, const automaton&) -> automaton);


      /*--------------.
      | dyn::shuffle. |
      `--------------*/

      template <typename Lhs, typename Rhs>
      automaton
      shuffle(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(shuffle(l, r));
      }

      REGISTER_DECLARE2(shuffle,
                        (const automaton&, const automaton&) -> automaton);


      /*--------------------.
      | dyn::infiltration.  |
      `--------------------*/

      template <typename Lhs, typename Rhs>
      automaton
      infiltration(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(infiltration(l, r));
      }

      REGISTER_DECLARE2(infiltration,
                        (const automaton&, const automaton&) -> automaton);


      /*-------------.
      | dyn::power.  |
      `-------------*/

      template <typename Aut>
      automaton
      power(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(power(a, n));
      }

      REGISTER_DECLARE(power,
                       (const automaton&, unsigned) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
