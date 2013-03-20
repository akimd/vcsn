#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------------------.
    | print(automaton, stream).  |
    `---------------------------*/

    std::ostream&
    print(const dyn::automaton& aut, std::ostream& out, const std::string& type)
    {
      if (type == "dot")
        dot(aut, out);
      else if (type == "fsm")
        fsm(aut, out);
      else if (type == "null")
        {}
      else
        throw std::domain_error("invalid output format for automaton: "
                                + type);
      return out;
    }


    /*------------------------.
    | print(ratexp, stream).  |
    `------------------------*/

    REGISTER_DEFINE(print_exp);

    std::ostream&
    print(const ratexp& exp, std::ostream& out, const std::string& type)
    {
      if (type == "text")
        details::print_exp_registry().call(exp->ctx().vname(), exp, out);
      else if(type == "xml")
        xml(exp, out);
      else
        throw std::domain_error("invalid output format for expression: "
                                + type);
      return out;
    }
  }
}
