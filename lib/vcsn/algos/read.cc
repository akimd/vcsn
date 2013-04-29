#include <cassert>
#include <stdexcept>

#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/driver.hh>

namespace vcsn
{

  namespace dyn
  {
    /*-----------------.
    | read_automaton.  |
    `-----------------*/

    automaton
    read_automaton_file(const std::string& f)
    {
      vcsn::detail::dot::driver d;
      auto res = d.parse_file(f);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
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
