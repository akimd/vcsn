#include <lib/vcsn/rat/read.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>
#include <lib/vcsn/rat/driver.hh>

namespace vcsn
{
  namespace rat
  {
    dyn::expression
    read(const dyn::context& ctx, rat::identities ids,
         std::istream& is, const location& l)
    {
      vcsn::rat::driver d{ctx, ids};
      auto res = d.parse(is, l);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }
  }
}
