#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <sstream>

# include <xercesc/dom/DOMImplementationRegistry.hpp>

# include <vcsn/algos/xml/ios.hh>
# include <vcsn/algos/xml/kratexpvisitor.hh>
# include <vcsn/algos/xml/structure.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/config.h>
# include <vcsn/core/rat/kratexp.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Context>
    void print(const Context& ctx, const rat::exp_t exp,
               xercesc::DOMDocument& doc, xercesc::DOMElement& node)
    {
      using xml_visitor = rat::xml_kratexp_visitor<Context>;
      using node_t = typename rat::xml_kratexp_visitor<Context>::node_t;

      auto rat_exp_node = details::create_node(doc, "expression");
      node.appendChild(rat_exp_node);
      auto value_type = details::create_node(doc, "valueType");
      rat_exp_node->appendChild(value_type);
      details::print_value_type(ctx, doc, *value_type);

      xml_visitor v(doc, *rat_exp_node, ctx);
      v(static_cast<const node_t&>(*exp));
    }

  } // namespace details

  template <class Context>
  class XML
  {
  public:
    using context_t = Context;

    XML(const context_t& ctx,
        const rat::exp_t exp) throw (xercesc::XMLException)
      : ctx_(ctx)
      , exp_ (exp)
    {
      xercesc::XMLPlatformUtils::Initialize();
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
      details::print(ctx_, exp_, *doc_, *root_);

      // Print in the stream
      details::print_xml(out, impl_, root_);
      // Print last end of line missing by Xerces
      out << std::endl;
    }

  private:
    void create_document()
    {
      using DomImplRes = xercesc::DOMImplementationRegistry;
      impl_ = DomImplRes::getDOMImplementation(details::transcode("LS"));
      doc_ = impl_->createDocument(details::transcode(VCSN_XMLNS),
                                   details::transcode("fsmxml"), nullptr);
      root_ = doc_->getDocumentElement();
    }

  private:
    const context_t& ctx_;
    const rat::exp_t exp_;
    xercesc::DOMImplementation* impl_;
    xercesc::DOMDocument* doc_;
    xercesc::DOMElement* root_;
  };

  template <class Context>
  inline
  void
  xml(std::ostream& out, const Context& ctx, const rat::exp_t exp)
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
    xml(o, ctx, exp);
    return o.str();
  }

  /*-------------.
  | abstract xml |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Ctx>
      std::string
      xml(const dyn::ratexp& exp)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp());
      }

      template <typename Ctx>
      void
      xml(std::ostream& out, const dyn::ratexp& exp)
      {
        return xml(out, dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp());
      }

      using xml_string_t =
        auto (const dyn::ratexp& e) -> std::string;

      using xml_stream_t =
        auto (std::ostream&out, const dyn::ratexp& e) -> void;

      bool xml_register(const std::string& ctx, const xml_stream_t& fn);
      bool xml_register(const std::string& ctx, const xml_string_t& fn);

    } // namespace details
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HH
