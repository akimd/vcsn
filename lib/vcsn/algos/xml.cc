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
    xml(const dyn::ratexp& e, std::ostream& out)
    {
      details::xml_stream_registry().call(e->ctx().vname(), e, out);
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
    xml(const dyn::ratexp& e)
    {
      return details::xml_registry().call(e->ctx().vname(), e);
    }

    /*------.
    | Read  |
    `------*/

    namespace details
    {
      Registry<xml_read_file_t>&
      xml_file_registry()
      {
        static Registry<xml_read_file_t> instance{"xml_read_file"};
        return instance;
      }

      bool xml_file_register(const std::string& ctx,
                              const xml_read_file_t& fn)
      {
        return xml_file_registry().set(ctx, fn);
      }
    } // namespace details

    ratexp
    xml_file(const context& ctx, const std::string& path)
    {
      return details::xml_file_registry().call(ctx.vname(), ctx, path);
    }

    namespace details
    {
      Registry<xml_read_string_t>&
      xml_string_registry()
      {
        static Registry<xml_read_string_t> instance{"xml_read_string"};
        return instance;
      }

      bool xml_string_register(const std::string& ctx,
                               const xml_read_string_t& fn)
      {
        return xml_string_registry().set(ctx, fn);
      }
    } // namespace details

    ratexp
    xml_string(const context& ctx, const std::string& exp)
    {
      return details::xml_string_registry().call(ctx.vname(), ctx, exp);
    }
  } // namespace dyn
} // namespace vcsn
