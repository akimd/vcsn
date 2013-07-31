#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/grail.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------.
    | fado.  |
    `-------*/

    REGISTER_DEFINE(fado);

    std::ostream&
    fado(const automaton& aut, std::ostream& out)
    {
      detail::fado_registry().call(aut->vname(false),
                                   aut, out);
      return out;
    }

    /*--------.
    | grail.  |
    `--------*/

    REGISTER_DEFINE(grail);

    std::ostream&
    grail(const automaton& aut, std::ostream& out)
    {
      detail::grail_registry().call(aut->vname(false),
                                    aut, out);
      return out;
    }
  }
}
