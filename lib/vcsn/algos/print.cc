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
    print(const dyn::automaton& aut, std::ostream& out, FileType type)
    {
      switch (type)
        {
        case FileType::dot:
          dot(aut, out);
          break;
        case FileType::fsm:
          fsm(aut, out);
          break;
        case FileType::null:
          break;
        case FileType::text:
          throw
            std::domain_error("invalid output format for automaton: text");
          break;
        case FileType::xml:
          //xml(out, aut); // FIXME:
          break;
        }
      return out;
    }


    /*------------------------.
    | print(ratexp, stream).  |
    `------------------------*/

    REGISTER_DEFINE(print_exp);

    std::ostream&
    print(const ratexp& exp, std::ostream& out, FileType type)
    {
      switch (type)
        {
        case FileType::dot:
          throw std::domain_error("invalid output format for expression: dot");
        case FileType::fsm:
          throw std::domain_error("invalid output format for expression: fsm");
        case FileType::null:
          break;
        case FileType::text:
          details::print_exp_registry().call(exp->ctx().vname(),
                                             exp, out);
          break;
        case FileType::xml:
          xml(exp, out);
          break;
        }
      return out;
    }
  }
}
