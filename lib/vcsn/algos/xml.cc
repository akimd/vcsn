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
    xml(const dyn::ratexp e, std::ostream& out)
    {
      details::xml_stream_registry().call(e->ctx().vname(), e, out);
    }

    /*-------------.
    | xml_string.  |
    `-------------*/

    namespace details
    {
      Registry<xml_string_t>&
      xml_write_string_registry()
      {
        static Registry<xml_string_t> instance{"xml_write_stream"};
        return instance;
      }

      bool xml_register(const std::string& ctx,
                   const xml_string_t& fn)
      {
        return xml_write_string_registry().set(ctx, fn);
      }
    } // namespace details

    std::string
    xml(const ratexp e)
    {
      return details::xml_write_string_registry().call(e->ctx().vname(), e);
    }

    namespace details
    {
      Registry<xml_read_stream_t>&
      xml_read_stream_registry()
      {
        static Registry<xml_read_stream_t> instance{"xml_read_stream"};
        return instance;
      }

      bool
      xml_register(const std::string& ctx,
                   const xml_read_stream_t& fn)
      {
        return xml_read_stream_registry().set(ctx, fn);
      }
    } // namespace details

    ratexp
    xml_read_stream(const context& ctx, std::istream& in)
    {
      return details::xml_read_stream_registry().call(ctx.vname(), ctx, in);
    }

    namespace details
    {
      Registry<xml_read_file_t>&
      xml_read_file_registry()
      {
        static Registry<xml_read_file_t> instance{"xml_read_file"};
        return instance;
      }

      bool
      xml_file_register(const std::string& ctx,
                             const xml_read_file_t& fn)
      {
        return xml_read_file_registry().set(ctx, fn);
      }
    } // namespace details

    ratexp
    xml_read_file(const context& ctx, const std::string& path)
    {
      return details::xml_read_file_registry().call(ctx.vname(), ctx, path);
    }

    namespace details
    {
      Registry<xml_read_string_t>&
      xml_read_string_registry()
      {
        static Registry<xml_read_string_t> instance{"xml_read_string"};
        return instance;
      }

      bool
      xml_string_register(const std::string& ctx,
                          const xml_read_string_t& fn)
      {
        return xml_read_string_registry().set(ctx, fn);
      }
    } // namespace details

    ratexp
    xml_read_string(const context& ctx, const std::string& exp)
    {
      return details::xml_read_string_registry().call(ctx.vname(), ctx, exp);
    }

  } // namespace dyn
} // namespace vcsn
