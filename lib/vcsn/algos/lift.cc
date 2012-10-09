#include <vcsn/algos/lift.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  Registry<lift_t>&
  lift_registry()
  {
    static Registry<lift_t> instance{"lift"};
    return instance;
  }

  bool lift_register(const std::string& ctx, const lift_t& fn)
  {
    return lift_registry().set(ctx, fn);
  }

  abstract_mutable_automaton*
  lift(const abstract_mutable_automaton& aut)
  {
    return lift_registry().call(aut.vname(),
                                aut);
  }
}
