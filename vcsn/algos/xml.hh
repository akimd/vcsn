#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <xercesc/dom/DOMImplementationRegistry.hpp>

# include <vcsn/core/rat/kratexp.hh>
# include <vcsn/algos/xml/kratexpvisitor.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/algos/xml/ios.hh>
# include <vcsn/config.h>
# include <vcsn/algos/xml/structure.hh>
// FIXME: # include <vcsn/algos/xml/exception.hh>

namespace vcsn
{

  namespace details
  {
    template <class Structure>
    void print(const Structure& s,
               xercesc::DOMDocument& doc,
               xercesc::DOMElement& n);

    template <typename Context>
    void print(const vcsn::mutable_automaton<Context> aut,
               xercesc::DOMDocument& doc,
               xercesc::DOMElement& node)
    {
      assert(false);
    }

    template <typename Context>
    void print(const std::pair<std::shared_ptr<const rat::exp>, Context> ratexp,
               xercesc::DOMDocument& doc,
               xercesc::DOMElement& node)
    {
      using xml_visitor = rat::xml_kratexp_visitor<Context>;
      using node_t = typename rat::xml_kratexp_visitor<Context>::node_t;

      auto rat_exp_node = details::create_node(doc, "expression");
      node.appendChild(rat_exp_node);
      auto value_type = details::create_node(doc, "valueType");
      rat_exp_node->appendChild(value_type);
      details::print_value_type(ratexp, doc, *value_type);

      xml_visitor v(doc, *rat_exp_node, ratexp.second);
      v(static_cast<const node_t&>(*ratexp.first));
    }

  } // namespace details

  template <class Structure>
  class XML
  {
  public:
    using structure_t = Structure;

    XML(const structure_t& s)
    try
      : s_(s)
    {
      xercesc::XMLPlatformUtils::Initialize();
    }
    catch (xercesc::XMLException& e)
    {
      //FIXME: throw details::XercesException("Initialization failed");
    }

    ~XML()
    {}

    void print(std::ostream& out)
    {
      create_document();
      // Set FsmXML version
      root_->setAttribute(details::transcode("version"),
                          details::transcode(FSMXML_VERSION));

      // Write the structure
      details::print(s_, *doc_, *root_);

      // Print in the stream
      details::print_xml(out, impl_, root_);
      // Print last end of line missing by Xerces
      out << std::endl;

    }

  private:
    void create_document()
    {
      impl_ =  xercesc::DOMImplementationRegistry::getDOMImplementation(details::transcode("LS"));
      doc_  = impl_->createDocument(details::transcode(VCSN_XMLNS),
                                    details::transcode("fsmxml"), nullptr);
      root_ = doc_->getDocumentElement();
    }
  private:
    const structure_t& s_;
    xercesc::DOMImplementation* impl_;
    xercesc::DOMDocument* doc_;
    xercesc::DOMElement* root_;
  };

  template <class Structure>
  inline
  void
  xml(std::ostream& out, const Structure& s)
  {
    XML<Structure> x(s);
    x.print(out);
  }

  template <class Structure>
  inline
  std::string
  xml(const Structure& s)
  {
    std::ostringstream o;
    xml(o, s);
    return o.str();
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HH
