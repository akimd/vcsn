#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/transpose.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------------------.
    | transpose for automata.  |
    `-------------------------*/

    namespace details
    {
      Registry<transpose_t>&
      transpose_registry()
      {
        static Registry<transpose_t> instance{"transpose"};
        return instance;
      }

      bool
      transpose_register(const std::string& ctx, const transpose_t& fn)
      {
        return transpose_registry().set(ctx, fn);
      }
    }

    automaton
    transpose(automaton& aut)
    {
      return details::transpose_registry().call(aut->vname(),
                                                aut);
    }

  }

    /*-----------------------.
    | transpose for ratexp.  |
    `-----------------------*/

  Registry<transpose_exp_t>&
  transpose_exp_registry()
  {
    static Registry<transpose_exp_t> instance{"transpose"};
    return instance;
  }

  bool
  transpose_exp_register(const std::string& ctx, const transpose_exp_t& fn)
  {
    return transpose_exp_registry().set(ctx, fn);
  }

  rat::exp_t
  transpose(const dyn::context& ctx, const rat::exp_t e)
  {
    return transpose_exp_registry().call(ctx.vname(),
                                         ctx, e);
  }
}
