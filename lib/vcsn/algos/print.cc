#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/algos/dyn.hh> // dotty
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------------------.
    | print(automaton, stream).  |
    `---------------------------*/

    std::ostream&
    print(const dyn::automaton& aut, std::ostream& out)
    {
      dotty(aut, out);
      return out;
    }


    /*------------------------.
    | print(ratexp, stream).  |
    `------------------------*/

    namespace details
    {
      Registry<print_exp_t>&
      print_exp_registry()
      {
        static Registry<print_exp_t> instance{"print_exp"};
        return instance;
      }

      bool print_exp_register(const std::string& ctx, const print_exp_t& fn)
      {
        return print_exp_registry().set(ctx, fn);
      }
    }

    std::ostream&
    print(const ratexp& exp, std::ostream& out)
    {
      return details::print_exp_registry().call(exp->ctx().vname(),
                                                exp, out);
    }
  }
}
