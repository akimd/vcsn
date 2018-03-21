#include <lib/vcsn/rat/read.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <lib/vcsn/rat/driver.hh>

namespace vcsn
{
  namespace rat
  {
    dyn::expression
    read(const dyn::context& ctx, rat::identities ids,
         std::istream& is, const location& l)
    {
      auto&& d = vcsn::rat::driver{ctx, ids};
      return d.parse(is, l);
    }
  }
}
