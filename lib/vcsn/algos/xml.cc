#include <vcsn/algos/xml.hh>
#include <vcsn/ctx/ctx.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------.
    | xml.  |
    `------*/

    REGISTER_DEFINE(xml);

    void
    xml(const dyn::ratexp& e, std::ostream& out)
    {
      detail::xml_registry().call(e->ctx().vname(), e, out);
    }

  } // namespace dyn
} // namespace vcsn
