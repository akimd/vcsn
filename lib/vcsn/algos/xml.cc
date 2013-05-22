#include <vcsn/algos/xml.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(xml);

    std::ostream&
    xml(const ratexp& e, std::ostream& out)
    {
      detail::xml_registry().call(e->ctx().vname(), e, out);
      return out;
    }

  } // namespace dyn
} // namespace vcsn
