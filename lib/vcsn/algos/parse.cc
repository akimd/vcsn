#include <vcsn/ctx/fwd.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/dot/driver.hh>

namespace vcsn
{

  namespace dyn
  {
    automaton
    parse_file(const std::string& f)
    {
      vcsn::dot::driver d;
      auto aut = d.parse_file(f);
      if (!d.errors.empty())
        throw new std::runtime_error(d.errors);
      return aut;
    }

    automaton
    parse_string(const std::string& s)
    {
      vcsn::dot::driver d;
      auto aut = d.parse_string(s);
      if (!d.errors.empty())
        throw new std::runtime_error(d.errors);
      return aut;
    }
  }

} // vcsn::
