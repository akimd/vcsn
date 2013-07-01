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
      else if (type == "info")
        info(aut, out);
      else if (type == "efsm")
        efsm(aut, out);
      else if (type == "null")
        {}
      else if (type == "tikz")
        tikz(aut, out);
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
      if (type == "info")
        info(exp, out);
      else if (type == "text")
        detail::print_exp_registry().call(exp->ctx().vname(), exp, out);
      else if(type == "xml")
        xml(exp, out);
      else
        throw std::domain_error("invalid output format for expression: "
                                + type);
      return out;
    }

    /*------------------------.
    | print(weight, stream).  |
    `------------------------*/

    REGISTER_DEFINE(print_weight);

    std::ostream&
    print(const dyn::weight& w, std::ostream& out, const std::string& type)
    {
      if (type == "text")
        detail::print_weight_registry().call(w->get_weightset().vname(),
                                             w, out);
      else
        throw std::domain_error("invalid output format for weight: "
                                + type);
      return out;
    }


  }
}
