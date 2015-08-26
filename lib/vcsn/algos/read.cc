#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/core/rat/expressionset.hh> // make_expressionset
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{

  namespace dyn
  {

    /*-----------------.
    | read_automaton.  |
    `-----------------*/

    namespace
    {
      automaton read_dot(std::istream& is)
      {
        vcsn::detail::dot::driver d;
        auto res = d.parse(is);
        if (!d.errors.empty())
          raise(d.errors);
        return res;
      }
    }

    automaton
    read_automaton(std::istream& is, const std::string& f)
    {
      enum fmt
      {
        dot,
        efsm,
        fado,
      };
      static const auto map = std::map<std::string, fmt>
        {
          {"default", dot},
          {"dot",     dot},
          {"efsm",    efsm},
          {"fado",    fado},
        };
      switch (getargs("automaton input format", map, f))
        {
        case dot:
          return read_dot(is);
        case efsm:
          return read_efsm(is);
        case fado:
          return read_fado(is);
        }
      BUILTIN_UNREACHABLE();
    }

    /*-------------------.
    | read_expression.   |
    `-------------------*/

    expression
    read_expression(const context& ctx, rat::identities ids,
                    std::istream& is, const std::string& f)
    {
      if (f == "text" || f == "default" || f == "")
        return rat::read(ctx, ids, is);
      else
        raise("invalid expression input format: ", f);
    }


    /*-------------.
    | read_label.  |
    `-------------*/

    REGISTER_DEFINE(read_label);
    label
    read_label(const dyn::context& ctx, std::istream& is)
    {
      return detail::read_label_registry().call(ctx, is);
    }

    /*------------------.
    | read_polynomial.  |
    `------------------*/

    REGISTER_DEFINE(read_polynomial);
    polynomial
    read_polynomial(const dyn::context& ctx, std::istream& is)
    {
      return detail::read_polynomial_registry().call(ctx, is);
    }

    /*--------------.
    | read_weight.  |
    `--------------*/

    REGISTER_DEFINE(read_weight);
    weight
    read_weight(const dyn::context& ctx, std::istream& is)
    {
      return detail::read_weight_registry().call(ctx, is);
    }

  }

} // vcsn::
