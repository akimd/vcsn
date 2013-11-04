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
    read_ratexp_file(const std::string& f, const ratexpset& rs,
                     const std::string& t)
    {
      if (t == "text" || t == "default" || t == "")
        return rat::read_file(f, rs);
      else
        throw std::domain_error("invalid input format for ratexp: " + t);
    }

    ratexp
    read_ratexp_string(const std::string& s, const ratexpset& rs,
                       const std::string& t)
    {
      if (t == "text" || t == "default" || t == "")
        return rat::read_string(s, rs);
      else
        throw std::domain_error("invalid input format for ratexp: " + t);
    }


    /*------------------.
    | read_polynomial.  |
    `------------------*/

    REGISTER_DEFINE(read_polynomial);

    static polynomial
    read_polynomial(const dyn::context& ctx, const std::string& s)
    {
      return detail::read_polynomial_registry().call(ctx->vname(false),
                                                 ctx, s);
    }

    polynomial
    read_polynomial_file(const std::string& f, const context& ctx)
    {
      return read_polynomial_string(get_file_contents(f), ctx);
    }

    polynomial
    read_polynomial_string(const std::string& s, const context& ctx)
    {
      return read_polynomial(ctx, s);
    }


    /*--------------.
    | read_weight.  |
    `--------------*/

    REGISTER_DEFINE(read_weight);

    static weight
    read_weight(const dyn::context& ctx, const std::string& s)
    {
      return detail::read_weight_registry().call(ctx->vname(false),
                                                 ctx, s);
    }

    weight
    read_weight_file(const std::string& f, const context& ctx)
    {
      return read_weight_string(get_file_contents(f), ctx);
    }

    weight
    read_weight_string(const std::string& s, const context& ctx)
    {
      return read_weight(ctx, s);
    }

  }

} // vcsn::
