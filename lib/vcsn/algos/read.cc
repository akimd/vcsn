#include <cassert>
#include <stdexcept>

#include <vcsn/ctx/fwd.hh>
#include <vcsn/algos/dyn.hh>
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
      vcsn::dot::driver d;
      auto res = d.parse_file(f);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }

    automaton
    read_automaton_string(const std::string& s)
    {
      vcsn::dot::driver d;
      auto res = d.parse_string(s);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }


    /*--------------.
    | read_ratexp.  |
    `--------------*/

    ratexp
    read_ratexp_file(const std::string& f,
                     const dyn::context& ctx,
                     FileType type)
    {
      switch (type)
        {
        case FileType::dotty:
          throw std::domain_error("Invalid input format for expression."
                                  " Could not read expression as dotty input.");
        case FileType::text:
          {
            vcsn::rat::driver d(ctx);
            auto exp = d.parse_file(f);
            if (!d.errors.empty())
              throw std::runtime_error(d.errors);
            return make_ratexp(ctx, exp);
          }
        case FileType::xml:
          return xml_read_file(ctx, f);
        }
    }

    ratexp
    read_ratexp_string(const std::string& s,
                       const dyn::context& ctx,
                       FileType type)
    {
      switch (type)
        {
        case FileType::dotty:
          throw std::domain_error("Invalid input format for expression."
                                  " Could not read expression as dotty input.");
        case FileType::text:
          {
            vcsn::rat::driver d(ctx);
            auto exp = d.parse_string(s);
            if (!d.errors.empty())
              throw std::runtime_error(d.errors);
            return make_ratexp(ctx, exp);
          }
        case FileType::xml:
          return xml_read_string(ctx, s);
        }
    }

  }

} // vcsn::
