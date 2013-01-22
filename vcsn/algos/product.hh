#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

#include <iostream>
#include <map>
#include <utility>
#include <deque>
#include "vcsn/core/mutable_automaton.hh"

namespace vcsn
{

  // This builds only the accessible part of the product.
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
    auto gs = intersect(*laut.context().labelset(), *raut.context().labelset());
    auto ls = std::make_shared<typename automaton_t::labelset_t>(gs);
    using context_t = typename automaton_t::context_t;
    auto ctx = context_t{ls, laut.context().weightset()};
    automaton_t aut(ctx);
    using state_t = typename automaton_t::state_t;

    using pair_t = std::pair<typename A::state_t, typename B::state_t>;
    std::map<pair_t, typename automaton_t::state_t> pmap;

    pair_t ppre(laut.pre(), raut.pre());
    pair_t ppost(laut.post(), raut.post());
    pmap[ppre] = aut.pre();
    pmap[ppost] = aut.post();

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
                auto weight = aut.weightset()->mul(lweight, raut.weight_of(ri));
                pair_t pdst(ldst, raut.dst_of(ri));

                auto iter = pmap.find(pdst);
                state_t dst;
                if (iter == pmap.end())
                  {
                    dst = aut.new_state();
                    pmap[pdst] = dst;
                    todo.push_back(pdst);
                  }
                else
                  {
                    dst = iter->second;
                  }
                aut.add_transition(src, dst, label, weight);
              }
          }
      }
    return aut;
  }

  /*---------------.
  | dyn::product.  |
  `---------------*/

  namespace dyn
  {
    namespace details
    {

      template <typename Lhs, typename Rhs>
      dyn::automaton
      product(const dyn::automaton& lhs, const dyn::automaton& rhs)
      {
        return
          std::make_shared<Lhs>(product(dynamic_cast<const Lhs&>(*lhs),
                                        dynamic_cast<const Rhs&>(*rhs)));
      }

      using product_t =
        auto (const dyn::automaton& lhs, const dyn::automaton& rhs)
        -> dyn::automaton;
      bool product_register(const std::string& lctx, const std::string& rctx,
                            const product_t& fn);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
