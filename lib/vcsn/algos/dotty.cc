#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*---------------.
    | dotty_stream.  |
    `---------------*/

    namespace details
    {
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
    }

    void
    dotty(const automaton& aut, std::ostream& out)
    {
      details::dotty_stream_registry().call(aut->vname(),
                                            aut, out);
    }

    /*---------------.
    | dotty_string.  |
    `---------------*/

    namespace details
    {
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
    }

    std::string
    dotty(const automaton& aut)
    {
      return details::dotty_string_registry().call(aut->vname(),
                                                   aut);
    }
  }
}
