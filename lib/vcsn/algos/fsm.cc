#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/fsm.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | fsm_stream.  |
    `-------------*/

    REGISTER_DEFINE(fsm_stream);

    void
    fsm(const automaton& aut, std::ostream& out)
    {
      details::fsm_stream_registry().call(aut->vname(),
                                          aut, out);
    }

    /*-------------.
    | fsm_string.  |
    `-------------*/

    REGISTER_DEFINE(fsm_string);

    std::string
    fsm(const automaton& aut)
    {
      return details::fsm_string_registry().call(aut->vname(),
                                                 aut);
    }
  }
}
