#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <sstream>

# include <xercesc/dom/DOMImplementationRegistry.hpp>

# include <vcsn/algos/xml/ios.hh>
# include <vcsn/algos/xml/ratexpvisitor.hh>
# include <vcsn/algos/xml/structure.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace detail
  {
    template <typename Context>
    void print(const Context& ctx, const rat::exp_t exp,
               xercesc::DOMDocument& doc, xercesc::DOMElement& node)
    {
      using xml_visitor = rat::xml_ratexp_visitor<Context>;
      using node_t = typename rat::xml_ratexp_visitor<Context>::node_t;

      auto rat_exp_node = detail::create_node(doc, "expression");
      node.appendChild(rat_exp_node);
      auto value_type = detail::create_node(doc, "valueType");
      rat_exp_node->appendChild(value_type);
      detail::print_value_type(ctx, doc, *value_type);

      xml_visitor v(doc, *rat_exp_node, ctx);
      v(static_cast<const node_t&>(*exp));
    }

  } // namespace detail

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
      root_->setAttribute(detail::transcode("version"),
                          detail::transcode(FSMXML_VERSION));

      // Write the structure
      detail::print(ctx_, exp_, *doc_, *root_);

      // Print in the stream
      detail::print_xml(out, impl_, root_);
      // Print last end of line missing by Xerces
      out << std::endl;
    }

  private:
    void create_document()
    {
      using DomImplRes = xercesc::DOMImplementationRegistry;
      impl_ = DomImplRes::getDOMImplementation(detail::transcode("LS"));
      doc_ = impl_->createDocument(detail::transcode(VCSN_XMLNS),
                                   detail::transcode("fsmxml"), nullptr);
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
  std::ostream&
  xml(const Context& ctx, const vcsn::rat::exp_t exp, std::ostream& out)
  {
    XML<Context> x(ctx, exp);
    x.print(out);
    return out;
  }

  /*-----------.
  | dyn::xml.  |
  `-----------*/

  namespace dyn
  {
    namespace detail
    {
      // ratexp, ostream -> ostream
      template <typename Ctx>
      std::ostream&
      xml(const ratexp& exp, std::ostream& out)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp(), out);
      }

      REGISTER_DECLARE(xml,
                       (const ratexp& e, std::ostream& out) -> std::ostream&);
    } // namespace detail
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HH
