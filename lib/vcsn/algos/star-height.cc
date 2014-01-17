#include <vcsn/dyn/ratexp.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/star-height.hh>
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
    star_height(const ratexp& e)
    {
      return detail::star_height_registry().call(e);
    }
  }
}
