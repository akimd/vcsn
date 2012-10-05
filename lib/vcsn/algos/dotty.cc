#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*---------------.
  | dotty_stream.  |
  `---------------*/

  Registry<dotty_stream_t>&
  dotty_stream_registry()
  {
    static Registry<dotty_stream_t> instance{"dotty_stream"};
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_stream_t& fn)
  {
    return dotty_stream_registry().set(ctx, fn);
  }

  void
  dotty(const abstract_mutable_automaton& aut, std::ostream& out)
  {
    dotty_stream_registry().call(aut.abstract_context().name(),
                                 aut, out);
  }

  /*---------------.
  | dotty_string.  |
  `---------------*/

  Registry<dotty_string_t>&
  dotty_string_registry()
  {
    static Registry<dotty_string_t> instance{"dotty_string"};
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_string_t& fn)
  {
    return dotty_string_registry().set(ctx, fn);
  }

  std::string
  dotty(const abstract_mutable_automaton& aut)
  {
    return dotty_string_registry().call(aut.abstract_context().name(),
                                        aut);
  }
}
