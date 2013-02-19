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
        case FileType::dot:
          throw std::domain_error("invalid input format for expression: dot");
        case FileType::fsm:
          throw std::domain_error("invalid input format for expression: fsm");
        case FileType::null:
          throw std::domain_error("invalid input format for expression: null");
        case FileType::text:
          {
            vcsn::rat::driver d(ctx);
            auto res = d.parse_file(f);
            if (!d.errors.empty())
              throw std::runtime_error(d.errors);
            return res;
          }
        case FileType::xml:
          throw std::domain_error("invalid input format for expression: xml");
          // FIXME: return xml_read_file(ctx, f);
        }
      abort();
    }

    ratexp
    read_ratexp_string(const std::string& s,
                       const dyn::context& ctx,
                       FileType type)
    {
      switch (type)
        {
        case FileType::dot:
          throw std::domain_error("invalid input format for expression: dot");
        case FileType::text:
          {
            vcsn::rat::driver d(ctx);
            auto res = d.parse_string(s);
            if (!d.errors.empty())
              throw std::runtime_error(d.errors);
            return res;
          }
        case FileType::fsm:
          throw std::domain_error("invalid input format for expression: fsm");
        case FileType::null:
          throw std::domain_error("invalid input format for expression: null");
        case FileType::xml:
          throw std::domain_error("invalid input format for expression: xml");
          // FIXME: return xml_read_string(ctx, s);
        }
      abort();
    }

  }

} // vcsn::
