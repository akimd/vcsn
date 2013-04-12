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

    void
    info(const automaton& aut, std::ostream& out)
    {
      details::info_registry().call(aut->vname(),
                                    aut, out);
    }

    /*---------------.
    | info(ratexp).  |
    `---------------*/

    REGISTER_DEFINE(info_exp);

    void
    info(const dyn::ratexp& e, std::ostream& out)
    {
      details::info_exp_registry().call(e->ctx().vname(),
                                        e, out);
    }
  }
}
