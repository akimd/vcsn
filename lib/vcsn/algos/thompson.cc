#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/thompson.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(thompson);

    automaton
    thompson(const dyn::ratexp& e)
    {
      return detail::thompson_registry().call(e);
    }
  }
}
