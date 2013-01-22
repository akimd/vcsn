#ifndef VCSN_ALGOS_XML_READ_HH
# define VCSN_ALGOS_XML_READ_HH

# include <vcsn/algos/xml/expression-handlers.hh>
# include <vcsn/algos/xml/error-handlers.hh>
# include <vcsn/algos/xml/fsmxml.hh>
# include <vcsn/algos/xml/value-type.hh>

namespace vcsn
{
  namespace details
  {
    template <typename Context>
    vcsn::rat::exp_t
    xml_file(const Context& ctx, const char* path)
    {
      xercesc::XMLPlatformUtils::Initialize();
      using namespace xercesc;
      // Create the parser
      auto parser = XMLReaderFactory::createXMLReader();

      // Define and set content and error handler
      details::FSMXMLHandler
        <StaticContentHandler<Context,
                              ExpressionHandler<Context>> > handler(ctx,
                                                                    *parser);
      details::XMLErrorHandler error;
      parser->setContentHandler(&handler);
      parser->setErrorHandler(&error);

      // Parse
      try
        {
          parser->parse(path);
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

  } // namespace details
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_READ_HH
