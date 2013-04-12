#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/info.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*--------------.
    | info_stream.  |
    `--------------*/

    REGISTER_DEFINE(info_stream);

    void
    info(const automaton& aut, std::ostream& out)
    {
      details::info_stream_registry().call(aut->vname(),
                                           aut, out);
    }

    /*-------------.
    | info_string.  |
    `-------------*/

    REGISTER_DEFINE(info_string);

    std::string
    info(const automaton& aut)
    {
      return details::info_string_registry().call(aut->vname(),
                                                  aut);
    }
  }
}
