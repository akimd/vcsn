#ifndef VCSN_ALGOS_XML_ERROR_HANDLERS_HH_
# define VCSN_ALGOS_XML_ERROR_HANDLERS_HH_

# include <stdexcept>

# include <xercesc/sax/HandlerBase.hpp>

# include <vcsn/algos/xml/tools.hh>

namespace vcsn
{
  namespace details
  {
    class XMLErrorHandler : public xercesc::HandlerBase
    {
    public:
      void startElement(const XMLCh* /* name */,
                        const xercesc::AttributeList& /* args */)
      {
      }

      void FatalError(const xercesc::SAXParseException& exception)
      {
        std::string what = "Fatal Error: ";
        what += transcode(exception.getMessage());
        what += " at line";
        what += exception.getLineNumber();

        throw std::domain_error(what);
      }
    };
  } // namespace details
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_ERROR_HANDLERS_HH_
