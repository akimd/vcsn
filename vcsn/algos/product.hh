#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

# include <deque>
# include <iostream>
# include <map>
# include <utility>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh>

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
      // FIXME: ensure that weightsets are compatible.
      static_assert(A::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(B::context_t::is_lal,
                    "requires labels_are_letters");
      using automaton_t = A;

      using state_t = typename automaton_t::state_t;
      using pair_t = std::pair<typename A::state_t, typename B::state_t>;

      /// Input automata, supplied at construction time.
      const A& laut_;
      const B& raut_;

      /// Map (left-state, right-state) -> product-state.
      using map = std::map<pair_t, typename automaton_t::state_t>;
      map pmap_;

      /// Worklist of (left-state, right-state).
      std::deque<pair_t> todo_;

      /// Add the given two source-automaton states to the
      /// worklist for the given result automaton if they aren't
      /// already there, updating the map; in any case return.
      state_t insert_if_needed(typename A::state_t lst, typename B::state_t rst,
                               automaton_t& res)
        ATTRIBUTE_HOT ATTRIBUTE_ALWAYS_INLINE
      {
        pair_t pdst(lst, rst);
        auto iter = pmap_.find(pdst);
        state_t dst;
        if (iter == pmap_.end())
          {
            dst = res.new_state();
            pmap_[pdst] = dst;
            todo_.emplace_back(pdst);
          }
        else
          dst = iter->second;
        return dst;
      }

    public:
      producter(const A& laut, const B& raut)
        : laut_(laut), raut_(raut)
      {
      }

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
        automaton_t res(ctx);

        pair_t ppre(laut_.pre(), raut_.pre());
        pair_t ppost(laut_.post(), raut_.post());
        pmap_[ppre] = res.pre();
        pmap_[ppost] = res.post();
        todo_.emplace_back(ppre);

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            for (auto li : laut_.all_out(psrc.first))
              {
                auto label = laut_.label_of(li);
                auto lweight = laut_.weight_of(li);
                auto ldst = laut_.dst_of(li);

                for (auto ri : raut_.out(psrc.second, label))
                  {
                    state_t dst = insert_if_needed(ldst, raut_.dst_of(ri), res);
                    res.add_transition(src, dst, label,
                                       ws.mul(lweight, raut_.weight_of(ri)));
                  }
              }
          }
        return res;
      }

      /// The (accessible part of the) shuffle of \a laut_ and \a raut_.
      automaton_t shuffle()
      {
        auto ctx = get_union(laut_.context(), raut_.context());
        const auto& ws = *ctx.weightset();
        automaton_t res(ctx);

        /// Make the result automaton initial states:
        for (auto lt : laut_.initial_transitions())
          for (auto rt : raut_.initial_transitions())
            {
              auto lsrc = laut_.dst_of(lt);
              auto rsrc = raut_.dst_of(rt);
              pair_t pair(lsrc, rsrc);
              state_t init = res.new_state();
              res.add_initial(init,
                              ws.mul(laut_.weight_of(lt),
                                     raut_.weight_of(rt)));
              pmap_[pair] = init;
              todo_.emplace_back(pair);
            }

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];
            state_t lsrc = psrc.first;
            state_t rsrc = psrc.second;
            if(laut_.is_final(lsrc) && raut_.is_final(rsrc))
              res.set_final(src,
                            ws.mul(laut_.get_final_weight(lsrc),
                                   raut_.get_final_weight(rsrc)));

            for (auto li : laut_.out(lsrc))
              {
                state_t dst = insert_if_needed(laut_.dst_of(li), rsrc, res);
                res.add_transition(src, dst, laut_.label_of(li), laut_.weight_of(li));
              }

            for (auto ri : raut_.out(rsrc))
              {
                state_t dst = insert_if_needed(lsrc, raut_.dst_of(ri), res);
                res.add_transition(src, dst, raut_.label_of(ri), raut_.weight_of(ri));
              }
          }
        return res;
      }

      /// The (accessible part of the) infiltration of \a laut_ and \a raut_.
      automaton_t infiltrate()
      {
        auto ctx = get_union(laut_.context(), raut_.context());
        const auto& ws = *ctx.weightset();
        automaton_t res(ctx);

        // Initialize the "shuffle" part:
        for (auto lt : laut_.initial_transitions())
          for (auto rt : raut_.initial_transitions())
            {
              auto lsrc = laut_.dst_of(lt);
              auto rsrc = raut_.dst_of(rt);
              pair_t pair(lsrc, rsrc);
              state_t init = res.new_state();
              res.add_initial(init,
                              ws.mul(laut_.weight_of(lt),
                                     raut_.weight_of(rt)));
              pmap_[pair] = init;
              todo_.emplace_back(pair);
            }

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            // Add new "product" successor states:
            for (auto li : laut_.out(psrc.first))
              {
                auto label = laut_.label_of(li);
                auto lweight = laut_.weight_of(li);
                auto ldst = laut_.dst_of(li);

                for (auto ri : raut_.out(psrc.second, label))
                  {
                    state_t dst = insert_if_needed(ldst, raut_.dst_of(ri), res);
                    res.add_transition(src, dst, label,
                                       ws.mul(lweight, raut_.weight_of(ri)));
                  }
              } // outer "product" for

            // Add new "infiltration" successor states:
            state_t lsrc = psrc.first;
            state_t rsrc = psrc.second;
            if(laut_.is_final(lsrc) && raut_.is_final(rsrc)){
              res.set_final(src,
                            ws.mul(laut_.get_final_weight(lsrc),
                                   raut_.get_final_weight(rsrc)));
            }
            for (auto li : laut_.out(lsrc))
              {
                state_t dst = insert_if_needed(laut_.dst_of(li), rsrc, res);
                res.add_transition(src, dst, laut_.label_of(li), laut_.weight_of(li));
              }
            for (auto ri : raut_.out(rsrc))
              {
                state_t dst = insert_if_needed(lsrc, raut_.dst_of(ri), res);
                res.add_transition(src, dst, raut_.label_of(ri), raut_.weight_of(ri));
              }
          } // while

        return res; // FIXME: this is a stub, of course
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
        o << "/* Origins." << std::endl;
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
    if (getenv("VCSN_PRODUCT"))
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
    if (getenv("VCSN_SHUFFLE"))
      product.print(std::cout, product.origins());
    return res;
  }


  /*------------.
  | infiltrate. |
  `------------*/

  /// Build the (accessible part of the) infiltration.
  template <typename A, typename B>
  A
  infiltrate(const A& laut, const B& raut)
  {
    detail::producter<A, B> product(laut, raut);
    auto res = product.infiltrate();
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_INFILTRATE"))
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
    switch (n)
      {
      case 0:
        {
          Aut res(aut.context());
          auto s = res.new_state();
          res.set_initial(s);
          res.set_final(s);
          for (auto l: *res.context().labelset())
            res.set_transition(s, s, l);
          return res;
        }
      case 1:
        return accessible(aut);
      default:
        auto t = power(aut, n / 2);
        auto t2 = product(t, t);
        if (n % 2 == 0)
          // n is even.
          return t2;
        else
          // n is odd.
          return product(t2, t);
      }
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
        return make_automaton(l.context(), product(l, r));
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
        return make_automaton(l.context(), shuffle(l, r));
      }

      REGISTER_DECLARE2(shuffle,
                        (const automaton&, const automaton&) -> automaton);


      /*-----------------.
      | dyn::infiltrate. |
      `-----------------*/

      template <typename Lhs, typename Rhs>
      automaton
      infiltrate(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(l.context(), infiltrate(l, r));
      }

      REGISTER_DECLARE2(infiltrate,
                        (const automaton&, const automaton&) -> automaton);


      /*-------------.
      | dyn::power.  |
      `-------------*/

      template <typename Aut>
      automaton
      power(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(a.context(), power(a, n));
      }

      REGISTER_DECLARE(power,
                       (const automaton&, unsigned) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
