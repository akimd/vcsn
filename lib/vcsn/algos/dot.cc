#include <vcsn/core/automaton.hh>
#include <vcsn/algos/dot.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | dot_stream.  |
    `-------------*/

    namespace details
    {
      Registry<dot_stream_t>&
      dot_stream_registry()
      {
        static Registry<dot_stream_t> instance{"dot_stream"};
        return instance;
      }

      bool dot_register(const std::string& ctx, const dot_stream_t& fn)
      {
        return dot_stream_registry().set(ctx, fn);
      }
    }

    void
    dot(const automaton& aut, std::ostream& out)
    {
      details::dot_stream_registry().call(aut->vname(),
                                          aut, out);
    }

    /*-------------.
    | dot_string.  |
    `-------------*/

    namespace details
    {
      Registry<dot_string_t>&
      dot_string_registry()
      {
        static Registry<dot_string_t> instance{"dot_string"};
        return instance;
      }

      bool dot_register(const std::string& ctx, const dot_string_t& fn)
      {
        return dot_string_registry().set(ctx, fn);
      }
    }

    std::string
    dot(const automaton& aut)
    {
      return details::dot_string_registry().call(aut->vname(),
                                                 aut);
    }
  }
}
