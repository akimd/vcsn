#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/algos/dyn.hh> // dot
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
        case FileType::text:
          throw
            std::domain_error("invalid output format for automaton."
                              " Could not print automaton as text output.");
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
    print(const ratexp& exp, std::ostream& out, FileType type)
    {
      switch (type)
        {
        case FileType::dot:
          std::domain_error("invalid output format for expression."
                            " Could not print expression as dot output.");
        case FileType::text:
          return details::print_exp_registry().call(exp->ctx().vname(),
                                                    exp, out);
        case FileType::xml:
          xml(exp, out);
          return out;
        }
      return out;
    }
  }
}
