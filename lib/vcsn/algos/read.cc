#include <cassert>
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
      automaton read_dot_file(const std::string& f)
      {
        vcsn::detail::dot::driver d;
        auto res = d.parse_file(f);
        if (!d.errors.empty())
          throw std::runtime_error(d.errors);
        return res;
      }
    }

    automaton
    read_automaton_file(const std::string& f, const std::string& t)
    {
      if (t == "dot" || t == "default" || t == "")
        return read_dot_file(f);
      else if (t == "efsm")
        return read_efsm_file(f);
      else if (t == "fado")
        return read_fado_file(f);
      throw std::runtime_error(t + ": unknown format");
    }

    automaton
    read_automaton_string(const std::string& s)
    {
      vcsn::detail::dot::driver d;
      auto res = d.parse_string(s);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
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
        throw std::domain_error("invalid input format for ratexp: " + t);
    }


    /*------------------.
    | read_polynomial.  |
    `------------------*/

    REGISTER_DEFINE(read_polynomial);

    polynomial
    read_polynomial(std::istream& is, const dyn::context& ctx)
    {
      return detail::read_polynomial_registry().call(ctx->vname(false),
                                                     ctx, is);
    }

    /*--------------.
    | read_weight.  |
    `--------------*/

    REGISTER_DEFINE(read_weight);

    weight
    read_weight(std::istream& is, const dyn::context& ctx)
    {
      return detail::read_weight_registry().call(ctx->vname(false),
                                                 ctx, is);
    }

  }

} // vcsn::
