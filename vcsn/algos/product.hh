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
    // FIXME: ensure that alphabets and weightsets are compatible.
    using automaton_t = A;
    automaton_t aut(laut.context());
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
}

#endif // !VCSN_ALGOS_PRODUCT_HH
