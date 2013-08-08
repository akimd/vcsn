#include <lib/vcsn/rat/read.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/ratexp.hh>
#include <vcsn/dyn/ratexpset.hh>
#include <lib/vcsn/rat/driver.hh>
#include <vcsn/dyn/algos.hh> // make_ratexpset

namespace vcsn
{
  namespace rat
  {
    // FIXME: we should pass a ratexpset, not a context, to the driver.

    dyn::ratexp
    read_file(const std::string& f, const dyn::context& ctx)
    {
      vcsn::rat::driver d(dyn::make_ratexpset(ctx));
      auto res = d.parse_file(f);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }

    dyn::ratexp
    read_string(const std::string& s, const dyn::ratexpset& rs)
    {
      vcsn::rat::driver d(rs);
      auto res = d.parse_string(s);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }

    dyn::ratexp
    read_string(const std::string& s, const dyn::context& ctx)
    {
      return read_string(s, dyn::make_ratexpset(ctx));
    }
  }
}
