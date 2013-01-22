#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <sstream>

# include <xercesc/sax2/XMLReaderFactory.hpp>

# include <vcsn/algos/xml/read.hh>
# include <vcsn/algos/xml/write.hh>

# include <vcsn/algos/xml/error-handlers.hh>
# include <vcsn/algos/xml/fsmxml.hh>
# include <vcsn/algos/xml/value-type.hh>
# include <vcsn/algos/xml/ios.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  template <class Context>
  inline
  void
  xml(const Context& ctx, const vcsn::rat::exp_t exp, std::ostream& out)
  {
    XML<Context> x(ctx, exp);
    x.print(out);
  }

  template <class Context>
  inline
  std::string
  xml(const Context& ctx, const rat::exp_t exp)
  {
    std::ostringstream o;
    xml(ctx, exp, o);
    return o.str();
  }


  /*-----------.
  | dyn::xml.  |
  `-----------*/

  namespace dyn
  {
    namespace details
    {
      // ratexp, ostream -> ostream
      template <typename Ctx>
      void
      xml(const dyn::ratexp& exp, std::ostream& out)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp(), out);
      }

      using xml_stream_t =
        auto (const dyn::ratexp& e, std::ostream& out) -> void;

      bool xml_register(const std::string& ctx, const xml_stream_t& fn);


      // ratexp -> string
      template <typename Ctx>
      std::string
      xml(const dyn::ratexp& exp)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp());
      }

      using xml_string_t = auto (const dyn::ratexp& e) -> std::string;

      bool xml_register(const std::string& ctx, const xml_string_t& fn);
    } // namespace details
  } // namespace dyn

  /*------------.
  | reading xml |
  `------------*/

  template <typename Context>
  vcsn::rat::exp_t
  xml(const Context& ctx, std::istream& in)
  {
    assert(false);
  }

  template <typename Context>
  vcsn::rat::exp_t
  xml_file(const Context& ctx, const std::string& path)
  {
    return details::xml_file(ctx, path.c_str());
  }

  template <typename Context>
  vcsn::rat::exp_t
  xml_string(const Context& ctx, const std::string& exp)
  {
    xercesc::XMLPlatformUtils::Initialize();
    using namespace xercesc;
    using namespace details;
    MemBufInputSource input((const XMLByte*) exp.c_str(),
                            exp.size(),
                            "xml_string",
                            false);
    // Create the parser
    auto parser = XMLReaderFactory::createXMLReader();
    // Define and set content and error handler
    FSMXMLHandler
      <StaticContentHandler<Context,
                            ExpressionHandler<Context>> > handler(ctx, *parser);
    XMLErrorHandler error;
    parser->setContentHandler(&handler);
    parser->setErrorHandler(&error);

    // Parse
    try
      {
        parser->parse(input);
      }
    catch (const xercesc::SAXParseException& e)
      {
        std::cerr << "catch this: " << e.getLineNumber() << ": "
                  << e.getColumnNumber() << ": "
                  << transcode(e.getMessage()) << std::endl;
        throw e;
      }

    // return the result
    return handler.get_parsed_content();
  }

  /*-------------.
  | abstract xml |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Ctx>
      vcsn::dyn::ratexp
      xml(const dyn::context& ctx, std::istream& in)
      {
        return make_ratexp(ctx, xml(dynamic_cast<const Ctx&>(ctx), in));
      }

      template <typename Ctx>
      vcsn::dyn::ratexp
      xml_file(const dyn::context& ctx, const std::string& path)
      {
        return make_ratexp(ctx, xml_file(dynamic_cast<const Ctx&>(ctx), path));
      }

      template <typename Ctx>
      vcsn::dyn::ratexp
      xml_string(const dyn::context& ctx, const std::string& exp)
      {
        return make_ratexp(ctx, xml_string(dynamic_cast<const Ctx&>(ctx), exp));
      }

      using xml_read_stream_t =
        auto (const dyn::context& ctx, std::istream& in)
        -> vcsn::dyn::ratexp;

      using xml_read_file_t =
        auto (const dyn::context& ctx, const std::string& path)
        -> vcsn::dyn::ratexp;

      using xml_read_string_t =
        auto (const dyn::context& ctx, const std::string& exp)
        -> vcsn::dyn::ratexp;

      bool xml_register(const std::string& cxt,
                        const xml_read_stream_t& fn);
      bool xml_file_register(const std::string& ctx,
                             const xml_read_file_t& fn);
      bool xml_string_register(const std::string& cxt,
                               const xml_read_string_t& fn);

    } // namespace details
  } // namespace dyn

} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HH
