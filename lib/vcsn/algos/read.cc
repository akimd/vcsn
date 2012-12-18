#include <cassert>
#include <stdexcept>

#include <vcsn/ctx/fwd.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/dot/driver.hh>

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

      if (!d.errors.empty())
        throw new std::runtime_error(d.errors);
      return aut;
    }
  }

} // vcsn::
