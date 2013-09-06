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

  /*----------.
  | product.  |
  `----------*/

  /// Build the (accessible part of the) product.
  template <class A, class B>
  A
  product(const A& laut, const B& raut)
  {
    static_assert(A::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(B::context_t::is_lal,
                  "requires labels_are_letters");
    // FIXME: ensure that weightsets are compatible.
    using automaton_t = A;
    auto ctx = intersection(laut.context(), raut.context());
    automaton_t res(ctx);
    using state_t = typename automaton_t::state_t;

    using pair_t = std::pair<typename A::state_t, typename B::state_t>;
    std::map<pair_t, typename automaton_t::state_t> pmap;

    pair_t ppre(laut.pre(), raut.pre());
    pair_t ppost(laut.post(), raut.post());
    pmap[ppre] = res.pre();
    pmap[ppost] = res.post();

    std::deque<pair_t> todo;
    todo.push_back(ppre);
    while (!todo.empty())
      {
        pair_t psrc = todo.front();
        todo.pop_front();
        state_t src = pmap[psrc];

        for (auto li : laut.all_out(psrc.first))
          {
            auto label = laut.label_of(li);
            auto lweight = laut.weight_of(li);
            auto ldst = laut.dst_of(li);

            for (auto ri : raut.out(psrc.second, label))
              {
                auto weight = res.weightset()->mul(lweight, raut.weight_of(ri));
                pair_t pdst(ldst, raut.dst_of(ri));

                auto iter = pmap.find(pdst);
                state_t dst;
                if (iter == pmap.end())
                  {
                    dst = res.new_state();
                    pmap[pdst] = dst;
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
