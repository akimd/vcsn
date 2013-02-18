#include <vcsn/core/automaton.hh>
#include <vcsn/algos/fsm.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | fsm_stream.  |
    `-------------*/

    namespace details
    {
      Registry<fsm_stream_t>&
      fsm_stream_registry()
      {
        static Registry<fsm_stream_t> instance{"fsm_stream"};
        return instance;
      }

      bool fsm_register(const std::string& ctx, const fsm_stream_t& fn)
      {
        return fsm_stream_registry().set(ctx, fn);
      }
    }

    void
    fsm(const automaton& aut, std::ostream& out)
    {
      details::fsm_stream_registry().call(aut->vname(),
                                          aut, out);
    }

    /*-------------.
    | fsm_string.  |
    `-------------*/

    namespace details
    {
      Registry<fsm_string_t>&
      fsm_string_registry()
      {
        static Registry<fsm_string_t> instance{"fsm_string"};
        return instance;
      }

      bool fsm_register(const std::string& ctx, const fsm_string_t& fn)
      {
        return fsm_string_registry().set(ctx, fn);
      }
    }

    std::string
    fsm(const automaton& aut)
    {
      return details::fsm_string_registry().call(aut->vname(),
                                                 aut);
    }
  }
}
