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
    dyn::ratexp read(std::istream& is, const dyn::ratexpset& rs,
                     const location& l)
    {
      vcsn::rat::driver d(rs);
      auto res = d.parse(is, l);
      if (!d.errors.empty())
        throw std::runtime_error(d.errors);
      return res;
    }
  }
}
