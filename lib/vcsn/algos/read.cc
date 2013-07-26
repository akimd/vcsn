#include <cassert>
#include <stdexcept>

#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/fado.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/driver.hh>

namespace vcsn
{

  namespace dyn
  {
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

      automaton read_fado_file(const std::string& f)
      {
        vcsn::detail::parse_fado d;
        auto res = d.parse_file(f);
        return res;
      }
    }

    automaton
    read_automaton_file(const std::string& f, const std::string& t)
    {
      if (t == "dot")
        return read_dot_file(f);
      if (t == "fado")
        return read_fado_file(f);
      if (t == "grail")
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
    read_ratexp_file(const std::string& f, const dyn::context& ctx,
                     const std::string& type)
    {
      if (type == "text")
        {
          vcsn::rat::driver d(ctx);
          auto res = d.parse_file(f);
          if (!d.errors.empty())
            throw std::runtime_error(d.errors);
          return res;
        }
      else
        throw std::domain_error("invalid input format for expression: "
                                + type);
    }

    ratexp
    read_ratexp_string(const std::string& s, const dyn::context& ctx,
                       const std::string& type)
    {
      if (type == "text")
        {
          vcsn::rat::driver d(ctx);
          auto res = d.parse_string(s);
          if (!d.errors.empty())
            throw std::runtime_error(d.errors);
          return res;
        }
      else
        throw std::domain_error("invalid input format for expression: "
                                + type);
    }

  }

} // vcsn::
