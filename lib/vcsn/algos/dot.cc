#include <vcsn/algos/dot.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | dot_stream.  |
    `-------------*/

    REGISTER_DEFINE(dot_stream);

    std::ostream&
    dot(const automaton& aut, std::ostream& out)
    {
      detail::dot_stream_registry().call(aut->vname(),
                                          aut, out);
      return out;
    }

    /*-------------.
    | dot_string.  |
    `-------------*/

    REGISTER_DEFINE(dot_string);

    std::string
    dot(const automaton& aut)
    {
      return detail::dot_string_registry().call(aut->vname(),
                                                 aut);
    }
  }
}
