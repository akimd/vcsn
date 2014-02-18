#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(edit);

    int
    edit(automaton& aut,
         int opcode, int int1, int int2, const std::string& label,
         const std::string& weight)
    {
      return detail::edit_registry().call(aut, opcode, int1, int2, label, weight);
    }
  }
}
