#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/xalloc.hh>

using namespace std::literals;

namespace vcsn
{
  namespace dyn
  {

    /*---------------------------.
    | print(expansion, stream).  |
    `---------------------------*/

    REGISTRY_DEFINE(print_expansion);

    std::ostream&
    print(const expansion& w, std::ostream& out,
          const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "text" || format == "utf8")
        detail::print_expansion_registry().call(w, out, format);
      else if (format == "default" || format == "")
        print(w, out, "utf8");
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
               || format == "ere"
               || format == "redgrep"
               || format == "latex"
               || format == "text"
               || format == "utf8")
        detail::print_expression_registry().call(exp, out, format);
      else if (format == "default")
        return print(exp, out, "utf8");
      else
        raise("invalid output format for expression: ", str_escape(format));
      return out;
    }


    /*-----------------------.
    | print(label, stream).  |
    `-----------------------*/

    REGISTRY_DEFINE(print_label);

    std::ostream&
    print(const label& w, std::ostream& out, const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "text" || format == "utf8")
        detail::print_label_registry().call(w, out, format);
      else if (format == "default" || format == "")
        return print(w, out, "utf8");
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
      else if (format == "latex" || format == "text" || format == "utf8")
        detail::print_polynomial_registry().call(p, out, format);
      else if (format == "default" || format == "")
        return print(p, out, "utf8");
      else
        raise("invalid output format for polynomial: ", str_escape(format));
      return out;
    }


    /*------------------------.
    | print(weight, stream).  |
    `------------------------*/

    REGISTRY_DEFINE(print_weight);

    std::ostream&
    print(const weight& w, std::ostream& out, const std::string& format)
    {
      if (format == "null")
        {}
      else if (format == "latex" || format == "text" || format == "utf8")
        detail::print_weight_registry().call(w, out, format);
      else if (format == "default" || format == "")
        return print(w, out, "utf8");
      else
        raise("invalid output format for weight: ", str_escape(format));
      return out;
    }


    /*-----------------.
    | ostream format.  |
    `-----------------*/

    xalloc<const std::string*> format_flag;

    void
    set_format(std::ostream& o, const std::string& format)
    {
      static auto formats = std::set<std::string>{};
      format_flag(o) = &*formats.insert(format).first;
    }

    std::string
    get_format(std::ostream& o)
    {
      return format_flag(o) ? *format_flag(o) : "default"s;
    }


    std::ostream&
    operator<<(std::ostream& o, const automaton& a)
    {
      return print(a, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const context& c)
    {
      return print(c, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const expansion& e)
    {
      return print(e, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const expression& r)
    {
      return print(r, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const label& l)
    {
      return print(l, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const polynomial& p)
    {
      return print(p, o, get_format(o));
    }

    std::ostream&
    operator<<(std::ostream& o, const weight& w)
    {
      return print(w, o, get_format(o));
    }
  }
}
