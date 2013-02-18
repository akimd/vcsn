#include <vcsn/core/automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | determinize.  |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<determinize_t>&
      determinize_registry()
      {
        static Registry<determinize_t> instance{"determinize"};
        return instance;
      }

      bool determinize_register(const std::string& ctx, const determinize_t& fn)
      {
        return determinize_registry().set(ctx, fn);
      }
    }

    automaton
    determinize(const automaton& aut)
    {
      return details::determinize_registry().call(aut->vname(),
                                                  aut);
    }
  }
}
