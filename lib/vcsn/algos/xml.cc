#include <vcsn/algos/xml.hh>
#include <vcsn/ctx/ctx.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | xml_stream.  |
    `-------------*/

    REGISTER_DEFINE(xml_stream); // namespace details

    void
    xml(const dyn::ratexp& e, std::ostream& out)
    {
      details::xml_stream_registry().call(e->ctx().vname(), e, out);
    }

    /*-------------.
    | xml_string.  |
    `-------------*/

    REGISTER_DEFINE(xml_string); // namespace details

    std::string
    xml(const dyn::ratexp& e)
    {
      return details::xml_string_registry().call(e->ctx().vname(), e);
    }

  } // namespace dyn
} // namespace vcsn
