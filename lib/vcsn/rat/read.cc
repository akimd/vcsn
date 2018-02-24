#include <lib/vcsn/rat/read.hh>

#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <lib/vcsn/rat/driver.hh>

namespace vcsn::rat
{
  dyn::expression
  read(const dyn::context& ctx, identities ids,
       std::istream& is,
       const std::string& format,
       const location& l)
  {
    auto d = vcsn::rat::driver{ctx, ids, format};
    return d.parse(is, l);
  }
}
