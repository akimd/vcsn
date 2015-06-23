#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registers.hh>

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
      if (f == "dot" || f == "default" || f == "")
        return read_dot(is);
      else if (f == "efsm")
        return read_efsm(is);
      else if (f == "fado")
        return read_fado(is);
      else
        raise("invalid automaton input format:", f);
    }

    /*-------------------.
    | read_expression.   |
    `-------------------*/

    expression
    read_expression(const expressionset& rs, std::istream& is,
                    const std::string& t)
    {
      if (t == "text" || t == "default" || t == "")
        return rat::read(rs, is);
      else
        raise("invalid expression input format: ", t);
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
