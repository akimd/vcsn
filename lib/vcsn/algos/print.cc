#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/xalloc.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------------------.
    | print(expansion, stream).  |
    `---------------------------*/

    REGISTRY_DEFINE(print_expansion);

    std::ostream&
    print(const dyn::expansion& w, std::ostream& out,
          const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "utf8")
        detail::print_expansion_registry().call(w, out, format);
      else if (format == "text" || format == "default" || format == "")
        {
          // FIXME: problem with rvalue if we pass
          // 'std::string("text")'.
          // FIXME: We _need_ the const, see name.hh.
          const std::string format = "text";
          detail::print_expansion_registry().call(w, out, format);
        }
      else
        raise("invalid output format for expansion: ", str_escape(format));
      return out;
    }

    /*-----------------------------.
    | print(expression, stream).   |
    `-----------------------------*/

    REGISTRY_DEFINE(print_expression);

    std::ostream&
    print(const expression& exp, std::ostream& out, const std::string& format)
    {
      if (format == "info")
        info(exp, out);
      else if (format == "null")
        {}
      else if (format == "dot"
               || format == "dot,logical" || format == "dot,physical"
               || format == "latex" || format == "utf8")
        detail::print_expression_registry().call(exp, out, format);
      else if (format == "text" || format == "default" || format == "")
        {
          // FIXME: problem with rvalue if we pass
          // 'std::string("text")'.
          // FIXME: We _need_ the const, see name.hh.
          const std::string format = "text";
          detail::print_expression_registry().call(exp, out, format);
        }
      else
        raise("invalid output format for expression: ", str_escape(format));
      return out;
    }


    /*-----------------------.
    | print(label, stream).  |
    `-----------------------*/

    REGISTRY_DEFINE(print_label);

    std::ostream&
    print(const dyn::label& w, std::ostream& out, const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "utf8")
        detail::print_label_registry().call(w, out, format);
      else if (format == "text" || format == "default" || format == "")
        {
          // FIXME: problem with rvalue if we pass
          // 'std::string("text")'.
          // FIXME: We _need_ the const, see name.hh.
          const std::string format = "text";
          detail::print_label_registry().call(w, out, format);
        }
      else
        raise("invalid output format for label: ", str_escape(format));
      return out;
    }

    /*----------------------------.
    | print(polynomial, stream).  |
    `----------------------------*/

    REGISTRY_DEFINE(list_polynomial);
    REGISTRY_DEFINE(print_polynomial);

    std::ostream&
    print(const polynomial& p, std::ostream& out, const std::string& format)
    {
      if (format == "list")
        detail::list_polynomial_registry().call(p, out);
      else if (format == "null")
        {}
      else if (format == "latex" || format == "utf8")
        detail::print_polynomial_registry().call(p, out, format);
      else if (format == "text" || format == "default" || format == "")
        {
          // FIXME: problem with rvalue if we pass
          // 'std::string("text")'.
          // FIXME: We _need_ the const, see name.hh.
          const std::string format = "text";
          detail::print_polynomial_registry().call(p, out, format);
        }
      else
        raise("invalid output format for polynomial: ", str_escape(format));
      return out;
    }


    /*------------------------.
    | print(weight, stream).  |
    `------------------------*/

    REGISTRY_DEFINE(print_weight);

    std::ostream&
    print(const dyn::weight& w, std::ostream& out, const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "utf8")
        detail::print_weight_registry().call(w, out, format);
      else if (format == "text" || format == "default" || format == "")
        {
          // FIXME: problem with rvalue if we pass
          // 'std::string("text")'.
          // FIXME: We _need_ the const, see name.hh.
          const std::string format = "text";
          detail::print_weight_registry().call(w, out, format);
        }
      else
        raise("invalid output format for weight: ", str_escape(format));
      return out;
    }


    /*-----------------.
    | ostream format.  |
    `-----------------*/

    xalloc<std::string*> format_flag;

    void
    set_format(std::ostream& o, const std::string& format)
    {
      if (!format_flag(o))
        format_flag(o) = new std::string{"default"};
      *format_flag(o) = format;
    }

    std::string
    get_format(std::ostream& o)
    {
      if (!format_flag(o))
        format_flag(o) = new std::string{"default"};
      return *format_flag(o);
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
  operator<<(std::ostream& o, const vcsn::dyn::context& c)
  {
    return vcsn::dyn::print(c, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::expansion& e)
  {
    return vcsn::dyn::print(e, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::expression& r)
  {
    return vcsn::dyn::print(r, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::label& l)
  {
    return vcsn::dyn::print(l, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::polynomial& p)
  {
    return vcsn::dyn::print(p, o, vcsn::dyn::get_format(o));
  }

  std::ostream&
  operator<<(std::ostream& o, const vcsn::dyn::weight& w)
  {
    return vcsn::dyn::print(w, o, vcsn::dyn::get_format(o));
  }

}
