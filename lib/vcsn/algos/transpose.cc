#include <vcsn/dyn/automaton.hh>
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


    /*-----------------------.
    | transpose for ratexp.  |
    `-----------------------*/

    namespace details
    {
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
    }

    ratexp
    transpose(const dyn::ratexp& e)
    {
      return details::transpose_exp_registry().call(e->ctx().vname(),
                                                    e);
    }
  }
}
