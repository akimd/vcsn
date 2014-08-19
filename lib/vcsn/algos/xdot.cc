#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/xdot.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(inject_xdot);

    automaton
    inject_xdot(const automaton& aut)
    {
      std::cerr << "OK-A 100 vcsn::dyn::inject_xdot\n";
      return detail::inject_xdot_registry().call(aut);
    }
  }
}
