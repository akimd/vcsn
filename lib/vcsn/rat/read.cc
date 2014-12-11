#include <lib/vcsn/rat/read.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/expressionset.hh>
#include <lib/vcsn/rat/driver.hh>
#include <vcsn/dyn/algos.hh> // make_expressionset

namespace vcsn
{
  namespace rat
  {
    dyn::expression
    read(const dyn::expressionset& rs, std::istream& is,
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
