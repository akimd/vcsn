#include <vcsn/algos/print.hh>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/xalloc.hh>

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
      if (type == "dot" || type == "default" || type == "")
        dot(aut, out);
      else if (type == "efsm")
        efsm(aut, out);
      else if (type == "fado")
        fado(aut, out);
      else if (type == "grail")
        grail(aut, out);
      else if (type == "info")
        info(aut, out);
      else if (type == "null")
        {}
      else if (type == "tikz")
        tikz(aut, out);
      else
        throw std::domain_error("invalid output format for automaton: "
                                + type);
      return out;
    }

    xalloc<std::string*> format_flag;

    void
    set_format(std::ostream& o, const std::string& type)
    {
      if (!format_flag(o))
        format_flag(o) = new std::string;
      *format_flag(o) = type;
    }

    std::string
    get_format(std::ostream& o)
    {
      if (!format_flag(o))
        format_flag(o) = new std::string;
      return *format_flag(o);
    }

    /*----------------------------.
    | print(polynomial, stream).  |
    `----------------------------*/

    REGISTER_DEFINE(list_polynomial);
    REGISTER_DEFINE(print_polynomial);

    std::ostream&
    print(const polynomial& p, std::ostream& out, const std::string& type)
    {
      if (type == "list")
        detail::list_polynomial_registry().call(p->vname(false), p, out);
      else if (type == "null")
        detail::print_polynomial_registry().call(p->vname(false), p, out);
      else if (type == "text" || type == "default" || type == "")
        detail::print_polynomial_registry().call(p->vname(false), p, out);
      else
        throw std::domain_error("invalid output format for polynonial: "
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
      else if (type == "null")
        {}
      else if (type == "text" || type == "default" || type == "")
        detail::print_exp_registry().call(exp->vname(false), exp, out);
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
      if (type == "null")
        {}
      else if (type == "text" || type == "default" || type == "")
        detail::print_weight_registry().call(w->vname(false),
                                             w, out);
      else
        throw std::domain_error("invalid output format for weight: "
                                + type);
      return out;
    }

  }
}

namespace std
{
  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::automaton& a)
  {
    return vcsn::dyn::print(a, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::polynomial& a)
  {
    return vcsn::dyn::print(a, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::ratexp& a)
  {
    return vcsn::dyn::print(a, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::weight& a)
  {
    return vcsn::dyn::print(a, o, vcsn::dyn::get_format(o));
  }

}
