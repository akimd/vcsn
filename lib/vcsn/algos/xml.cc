#include <vcsn/algos/xml.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | xml_stream.  |
    `-------------*/

    namespace details
    {
      Registry<xml_stream_t>&
      xml_stream_registry()
      {
        static Registry<xml_stream_t> instance{"xml_stream"};
        return instance;
      }

      bool
      xml_register(const std::string& ctx, const xml_stream_t& fn)
      {
        return xml_stream_registry().set(ctx, fn);
      }
    } // namespace details

    void
    xml(const std::string& ctx, const dyn::ratexp& e, std::ostream& out)
    {
      details::xml_stream_registry().call(ctx, out, e);
    }

    /*-------------.
    | xml_string.  |
    `-------------*/

    namespace details
    {
      Registry<xml_string_t>&
      xml_string_registry()
      {
        static Registry<xml_string_t> instance{"xml_stream"};
        return instance;
      }

      bool xml_register(const std::string& ctx, const xml_string_t& fn)
      {
        return xml_string_registry().set(ctx, fn);
      }
    } // namespace details

    std::string
    xml(const std::string& ctx, const dyn::ratexp& e)
    {
      return details::xml_string_registry().call(ctx, e);
    }

  } // namespace dyn
} // namespace vcsn
