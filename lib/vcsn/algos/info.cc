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
    info(const automaton& aut, std::ostream& out)
    {
      detail::info_registry().call(aut->vname(),
                                   aut, out);
      return out;
    }

    /*---------------.
    | info(ratexp).  |
    `---------------*/

    REGISTER_DEFINE(info_exp);

    std::ostream&
    info(const dyn::ratexp& e, std::ostream& out)
    {
      detail::info_exp_registry().call(e->ctx().vname(),
                                       e, out);
      return out;
    }
  }
}
