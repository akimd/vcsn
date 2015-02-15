#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/expression.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(constant_term);
    weight
    constant_term(const expression& e)
    {
      return detail::constant_term_registry().call(e);
    }
  }
}
