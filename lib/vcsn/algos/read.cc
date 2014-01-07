#include <stdexcept>

#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/read.hh>
#include <lib/vcsn/rat/read.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>

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
    read_automaton(std::istream& is, const std::string& t)
    {
      if (t == "dot" || t == "default" || t == "")
        return read_dot(is);
      else if (t == "efsm")
        return read_efsm(is);
      else if (t == "fado")
        return read_fado(is);
      else
        raise(t + ": unknown format");
    }

    /*--------------.
    | read_ratexp.  |
    `--------------*/

    ratexp
    read_ratexp(std::istream& is, const ratexpset& rs,
                const std::string& t)
    {
      if (t == "text" || t == "default" || t == "")
        return rat::read(is, rs);
      else
        raise("invalid input format for ratexp: ", t);
    }


    /*------------------.
    | read_polynomial.  |
    `------------------*/

    REGISTER_DEFINE(read_polynomial);

    polynomial
    read_polynomial(std::istream& is, const dyn::context& ctx)
    {
      return detail::read_polynomial_registry().call(ctx, is);
    }

    /*--------------.
    | read_weight.  |
    `--------------*/

    REGISTER_DEFINE(read_weight);

    weight
    read_weight(std::istream& is, const dyn::context& ctx)
    {
      return detail::read_weight_registry().call(ctx, is);
    }

  }

} // vcsn::
