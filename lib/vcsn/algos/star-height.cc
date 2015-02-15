#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registers.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    /*-------------------.
    | star-height(exp).  |
    `-------------------*/
    REGISTER_DEFINE(star_height);

    unsigned
    star_height(const expression& e)
    {
      return detail::star_height_registry().call(e);
    }
  }
}
