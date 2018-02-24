#include <lib/vcsn/dot/read.hh>

#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/dot/driver.hh>

namespace vcsn::detail::dot
{
  dyn::automaton read(std::istream& is, const location&)
  {
    auto d = vcsn::detail::dot::driver{};
    return d.parse(is);
  }
}
