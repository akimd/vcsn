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

      /// map (left-state, right-state) -> product-state.
      using map = std::map<pair_t, typename automaton_t::state_t>;
      map pmap_;

    public:
      /// Reset the attributes before a new product.
      void clear()
      {
        pmap_.clear();
      }

      /// The (accessible part of the) product of \a laut and \a raut.
      automaton_t operator()(const A& laut, const B& raut)
      {
        auto ctx = intersection(laut.context(), raut.context());
        const auto& ws = *ctx.weightset();
        automaton_t res(ctx);

        pair_t ppre(laut.pre(), raut.pre());
        pair_t ppost(laut.post(), raut.post());
        pmap_[ppre] = res.pre();
        pmap_[ppost] = res.post();

        /// Worklist of (left-state, right-state).
        std::deque<pair_t> todo;
        todo.push_back(ppre);
        while (!todo.empty())
          {
            pair_t psrc = todo.front();
            todo.pop_front();
            state_t src = pmap_[psrc];

            for (auto li : laut.all_out(psrc.first))
              {
                auto label = laut.label_of(li);
                auto lweight = laut.weight_of(li);
                auto ldst = laut.dst_of(li);

                for (auto ri : raut.out(psrc.second, label))
                  {
                    auto weight = ws.mul(lweight, raut.weight_of(ri));
                    pair_t pdst(ldst, raut.dst_of(ri));

                    auto iter = pmap_.find(pdst);
                    state_t dst;
                    if (iter == pmap_.end())
                      {
                        dst = res.new_state();
                        pmap_[pdst] = dst;
                        todo.push_back(pdst);
                      }
                    else
                      {
                        dst = iter->second;
                      }
                    res.add_transition(src, dst, label, weight);
                  }
              }
          }
        return res;
      }
    };
  }

  /// Build the (accessible part of the) product.
  template <typename A, typename B>
  A
  product(const A& laut, const B& raut)
  {
    detail::producter<A, B> product;
    return product(laut, raut);
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
