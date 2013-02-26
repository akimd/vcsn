#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/eval.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {

    /*-------.
    | eval.  |
    `-------*/

    namespace details
    {
      Registry<eval_t>&
      eval_registry()
      {
        static Registry<eval_t> instance{"eval"};
        return instance;
      }

      bool eval_register(const std::string& ctx, const eval_t& fn)
      {
        return eval_registry().set(ctx, fn);
      }
    }

    std::string
    eval(const automaton& aut, const std::string& s)
    {
      return details::eval_registry().call(aut->vname(),
                                           aut, s);
    }
  }
}
