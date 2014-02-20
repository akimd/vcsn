#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

# include <deque>
# include <iostream>
# include <map>
# include <utility>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/ctx/context.hh>

namespace vcsn
{

  namespace detail
  {

    /*----------------------------------.
    | producter<automaton, automaton>.  |
    `----------------------------------*/

    /// Build the (accessible part of the) product.
    template <typename Lhs, typename Rhs>
    class producter
    {
      static_assert(Lhs::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(Rhs::context_t::is_lal,
                    "requires labels_are_letters");

      // The _type_ of the context is the "union" of the contexts,
      // independently of the algorithm.  However, its _value_
      // differs: in the case of the product, the labelset is the meet
      // of the labelsets, it is its join for shuffle and
      // infiltration.
      using context_t = join_t<typename Lhs::context_t,
                               typename Rhs::context_t>;
      using labelset_t = typename context_t::labelset_t;
      using weightset_t = typename context_t::weightset_t;

    public:
      using automaton_t = mutable_automaton<context_t>;

      producter(const Lhs& lhs, const Rhs& rhs)
        : lhs_(std::move(sort(lhs))), rhs_(std::move(sort(rhs))),
          res_(join(lhs_.context(), rhs_.context()))
      {}

      /// Reset the attributes before a new product.
      void clear()
      {
        pmap_.clear();
        todo_.clear();
      }

      /// The (accessible part of the) product of \a lhs_ and \a rhs_.
      automaton_t product()
      {
        auto ctx = meet(lhs_.context(), rhs_.context());
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

      /// The (accessible part of the) shuffle product of \a lhs_ and
      /// \a rhs_.
      automaton_t shuffle()
      {
        auto ctx = join(lhs_.context(), rhs_.context());
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

      /// The (accessible part of the) infiltration product of \a
      /// lhs_ and \a rhs_.
      automaton_t infiltration()
      {
        auto ctx = join(lhs_.context(), rhs_.context());
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
            //
            // Product transitions must be added before shuffle ones:
            // this way "product" can use "new_transition" only, which
            // is faster than "add_transition".
            add_product_transitions(ws, src, psrc);
            add_shuffle_transitions(ws, src, psrc);
          }

        return std::move(res_);
      }

      /// A map from product states to pair of original states.
      using state_t = typename automaton_t::state_t;
      using pair_t = std::pair<typename Lhs::state_t, typename Rhs::state_t>;
      using origins_t = std::map<state_t, pair_t>;
      origins_t
      origins() const
      {
        origins_t res;
        for (const auto& p: pmap_)
          res.emplace(p.second, p.first);
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n";
        for (auto p: orig)
          if (p.first != automaton_t::pre() && p.first != automaton_t::post())
            o << "    " << p.first - 2
              << " [label = \""
              << p.second.first - 2
              << ','
              << p.second.second - 2
              << "\"]\n";
        o << "*/\n";
        return o;
      }

    private:
      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// Input automata, supplied at construction time.
      const Lhs lhs_;
      const Rhs rhs_;

      /// Map (left-state, right-state) -> product-state.
      using map = std::map<pair_t, state_t>;
      map pmap_;

      /// Worklist of (left-state, right-state).
      std::deque<pair_t> todo_;

      /// Add the pre and post states in the result automaton.  This
      /// is needed for all three algorithms here.
      void initialize()
      {
        pair_t ppre(lhs_.pre(), rhs_.pre());
        pair_t ppost(lhs_.post(), rhs_.post());
        pmap_[ppre] = res_.pre();
        pmap_[ppost] = res_.post();
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_product()
      {
        initialize();
        todo_.emplace_back(pair_t(lhs_.pre(), rhs_.pre()));
      }

      /// The product between two weights, possibly from different
      /// weightsets.
      weight_t mul_(const weightset_t& ws,
                    const typename Lhs::weight_t l,
                    const typename Rhs::weight_t r) const
      {
        return ws.mul(ws.conv(*lhs_.weightset(), l),
                      ws.conv(*rhs_.weightset(), r));
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle(const weightset_t& ws)
      {
        initialize();

        /// Make the result automaton initial states:
        for (auto lt : lhs_.initial_transitions())
          for (auto rt : rhs_.initial_transitions())
            {
              auto lsrc = lhs_.dst_of(lt);
              auto rsrc = rhs_.dst_of(rt);
              pair_t pair(lsrc, rsrc);
              state_t init = res_.new_state();
              res_.add_initial(init,
                               mul_(ws,
                                    lhs_.weight_of(lt),
                                    rhs_.weight_of(rt)));
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
      state_t state(typename Lhs::state_t lst, typename Rhs::state_t rst)
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
      /// If needed, push the destination state in the work list.
      /// \pre !res.has_transition(src, dst, label).
      void
      new_transition(state_t src,
                     typename Lhs::state_t ldst, typename Rhs::state_t rdst,
                     const label_t& label, const weight_t& weight)
      {
        res_.new_transition(src, state(ldst, rdst), label, weight);
      }

      /// Add a transition in the result from destination states in operands.
      /// If needed, push the destination state in the work list.
      void
      add_transition(state_t src,
                     typename Lhs::state_t ldst, typename Rhs::state_t rdst,
                     const label_t& label, const weight_t& weight)
      {
        res_.add_transition(src, state(ldst, rdst), label, weight);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_product_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        // This relies on outgoing transitions being sorted by label
        // by the sort algorithm: thanks to that property we can scan
        // the two successor lists in lockstep.
        auto ls = lhs_.all_out(psrc.first);
        auto rs = rhs_.all_out(psrc.second);
        auto li = ls.begin();
        auto ri = rs.begin();
        for (/* Nothing. */;
             li != ls.end() && ri != rs.end();
             ++ li)
          {
            auto lt = *li;
            label_t label = lhs_.label_of(lt);
            // Skip right-hand transitions with labels we don't have
            // on the left hand.
            while (labelset_t::less_than(rhs_.label_of(*ri), label))
              if (++ ri == rs.end())
                return;

            // If the smallest label on the right-hand side is bigger
            // than the left-hand one, we have no hope of ever adding
            // transitions with this label.
            if (labelset_t::less_than(label, rhs_.label_of(*ri)))
              continue;

            assert(labelset_t::equals(label, rhs_.label_of(*ri)));
            auto rstart = ri;
            while (labelset_t::equals(rhs_.label_of(*ri), label))
              {
                // These are always new transitions: first because the
                // source state is visited for the first time, and
                // second because the couple (left destination, label)
                // is unique, and so is (right destination, label).
                new_transition(src, lhs_.dst_of(lt), rhs_.dst_of(*ri), label,
                               mul_(ws,
                                    lhs_.weight_of(lt), rhs_.weight_of(*ri)));

                if (++ ri == rs.end())
                  break;
              }

            // Move the right-hand iterator back to the beginning of
            // the matching part.  This will be needed if the next
            // left-hand transition has the same label.
            ri = rstart;
          }
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// givenpair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_shuffle_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        typename Lhs::state_t lsrc = psrc.first;
        typename Rhs::state_t rsrc = psrc.second;
        if (lhs_.is_final(lsrc) && rhs_.is_final(rsrc))
          res_.set_final(src,
                         mul_(ws,
                              lhs_.get_final_weight(lsrc),
                              rhs_.get_final_weight(rsrc)));

        // The src state is visited for the first time, so all these
        // transitions are new.  *Except* in the case where we have a
        // loop on both the lhs, and the rhs.
        //
        // If add_product_transitions was called before, there may
        // even exist such a transition in the first loop.
        for (auto lt : lhs_.out(lsrc))
          {
            typename Lhs::state_t ldst = lhs_.dst_of(lt);
            if (lsrc == ldst)
              add_transition(src, lhs_.dst_of(lt), rsrc,
                             lhs_.label_of(lt),
                             ws.conv(*lhs_.weightset(), lhs_.weight_of(lt)));
            else
              new_transition(src, lhs_.dst_of(lt), rsrc,
                             lhs_.label_of(lt),
                             ws.conv(*lhs_.weightset(), lhs_.weight_of(lt)));
          }
        for (auto rt : rhs_.out(rsrc))
          {
            typename Rhs::state_t rdst = rhs_.dst_of(rt);
            if (rsrc == rdst)
              add_transition(src, lsrc, rdst,
                             rhs_.label_of(rt),
                             ws.conv(*rhs_.weightset(), rhs_.weight_of(rt)));
            else
              new_transition(src, lsrc, rdst,
                             rhs_.label_of(rt),
                             ws.conv(*rhs_.weightset(), rhs_.weight_of(rt)));
          }
      }

    private:
      /// The computed product.
      automaton_t res_;
    };
  }

  /*--------------------------------.
  | product(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) product.
  template <typename Lhs, typename Rhs>
  auto
  product(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.product();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      product(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(product(l, r));
      }

      REGISTER_DECLARE(product,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*--------------------------------.
  | shuffle(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) shuffle.
  template <typename Lhs, typename Rhs>
  auto
  shuffle(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.shuffle();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      shuffle(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(shuffle(l, r));
      }

      REGISTER_DECLARE(shuffle,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*-------------------------------------.
  | infiltration(automaton, automaton).  |
  `-------------------------------------*/

  /// Build the (accessible part of the) infiltration.
  template <typename Lhs, typename Rhs>
  auto
  infiltration(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.infiltration();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      infiltration(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(infiltration(l, r));
      }

      REGISTER_DECLARE(infiltration,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*----------------------.
  | power(automaton, n).  |
  `----------------------*/

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
        res.new_transition(s, s, l);
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
      /// Bridge.
      template <typename Aut, typename Unsigned>
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


  /*-------------------------------.
  | intersection(ratexp, ratexp).  |
  `-------------------------------*/

  /// Intersection/Hadamard product of ratexps.
  template <typename RatExpSet>
  inline
  typename RatExpSet::ratexp_t
  intersection(const RatExpSet& rs,
               const typename RatExpSet::ratexp_t& lhs,
               const typename RatExpSet::ratexp_t& rhs)
  {
    return rs.intersection(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      intersection_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetLhs>();
        return make_ratexp(l.get_ratexpset(),
                           ::vcsn::intersection<RatExpSetLhs>(l.get_ratexpset(),
                                                              l.ratexp(),
                                                              r.ratexp()));
      }

      REGISTER_DECLARE(intersection_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
