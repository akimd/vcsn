#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | determinize.  |
  `-------------*/

  Registry<determinize_t>&
  determinize_registry()
  {
    static Registry<determinize_t> instance{"determinize"};
    return instance;
  }

  bool determinize_register(const std::string& ctx, const determinize_t& fn)
  {
    return determinize_registry().set(ctx, fn);
  }

  dyn::abstract_automaton*
  determinize(const dyn::abstract_automaton& aut)
  {
    return determinize_registry().call(aut.vname(),
                                       aut);
  }
}
