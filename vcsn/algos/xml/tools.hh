#ifndef VCSN_ALGOS_XML_TOOLS_HH
# define VCSN_ALGOS_XML_TOOLS_HH

# include <xercesc/dom/DOM.hpp>
// # include <xercesc/util/XMLString.hpp>
// # include <xercesc/util/PlatformUtils.hpp>

# include <vcsn/config.h>

namespace vcsn
{
  namespace details
  {
    inline
    XMLCh*
    transcode (const char* ccp)
    {
      return xercesc::XMLString::transcode(ccp);
    }

    inline
    XMLCh*
    transcode(const std::string& s)
    {
      return transcode(s.c_str());
    }

    inline
    xercesc::DOMElement*
    create_node(xercesc::DOMDocument& doc, const std::string& e)
    {
      return doc.createElementNS(transcode(VCSN_XMLNS), transcode(e));
    }

    inline
    void
    set_attribute(xercesc::DOMElement* node,
                  const std::string& label,
                  const std::string& value)
    {
      if (!value.empty())
        node->setAttribute(transcode(label), transcode(value));
    }

  } // namespace xml
} // namespace vcsn
#endif // !VCSN_ALGOS_XML_TOOLS_HH
