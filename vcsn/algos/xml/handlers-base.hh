#ifndef VCSN_ALGOS_XML_HANDLERS_BASE_HH
# define VCSN_ALGOS_XML_HANDLERS_BASE_HH

// FIXME: Clean headers
# include <xercesc/sax2/SAX2XMLReader.hpp>
# include <xercesc/sax2/XMLReaderFactory.hpp>
# include <xercesc/sax2/DefaultHandler.hpp>
# include <xercesc/sax2/Attributes.hpp>

# include <xercesc/sax/SAXException.hpp>
# include <xercesc/util/XMLString.hpp>

# include <vcsn/algos/xml/xml-eq.hh>

namespace vcsn
{
  namespace details
  {
    class Handler : public xercesc::DefaultHandler
    {
    public:
      using handler_t = xercesc::DefaultHandler;
      using string = const XMLCh* const;
      using attributes = const xercesc::Attributes&;
      using parser_t = xercesc::SAX2XMLReader;
      using xsize_t = XMLSize_t;
    public:
      Handler(parser_t& parser)
        : parser_(parser)
      {}
      void startElement(string uri, string localname, string qname,
                        attributes attrs)
      {
        start(uri, localname, qname, attrs);
      }
      void endElement(string uri, string localname, string qname)
      {
        end(uri, localname, qname);
      }
      virtual void start(string uri, string localname, string qname,
                         attributes attrs) = 0;
      virtual void end(string uri, string localname, string qname) = 0;
    protected:
      xmlEq eq_;
      parser_t& parser_;
    };

  } // namespace details

} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HANDLERS_BASE_HH
