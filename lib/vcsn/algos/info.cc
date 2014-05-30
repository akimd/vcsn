#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/info.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------------------.
    | info(automaton).  |
    `------------------*/

    REGISTER_DEFINE(info);

    std::ostream&
    info(const automaton& aut, std::ostream& out, bool detailed)
    {
      detail::info_registry().call(aut, out, detailed);
      return out;
    }

    /*---------------.
    | info(ratexp).  |
    `---------------*/

    REGISTER_DEFINE(info_ratexp);

    std::ostream&
    info(const dyn::ratexp& e, std::ostream& out)
    {
      detail::info_ratexp_registry().call(e, out);
      return out;
    }
  }
}
