#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <sstream>
# include <fstream>

# include <xercesc/dom/DOMImplementationRegistry.hpp>
# include <xercesc/sax2/XMLReaderFactory.hpp>
# include <xercesc/framework/MemBufInputSource.hpp>

# include <vcsn/algos/xml/expression-handlers.hh>
# include <vcsn/algos/xml/error-handlers.hh>
# include <vcsn/algos/xml/ios.hh>
# include <vcsn/algos/xml/ratexpvisitor.hh>
# include <vcsn/algos/xml/structure.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/config.h>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Context>
    void print(const Context& ctx, const rat::exp_t exp,
               xercesc::DOMDocument& doc, xercesc::DOMElement& node)
    {
      using xml_visitor = rat::xml_ratexp_visitor<Context>;
      using node_t = typename rat::xml_ratexp_visitor<Context>::node_t;

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

  /*-------------.
  | abstract xml |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Ctx>
      std::string
      xml(const dyn::ratexp exp)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp());
      }

      template <typename Ctx>
      void
      xml(const dyn::ratexp exp, std::ostream& out)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp(), out);
      }

      using xml_string_t =
        auto (const dyn::ratexp e) -> std::string;

      using xml_stream_t =
        auto (const dyn::ratexp e, std::ostream& out) -> void;

      bool xml_register(const std::string& ctx, const xml_stream_t& fn);
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

  namespace details
  {

    template <typename Context>
    vcsn::rat::exp_t
    xml_file(const Context& ctx, const char* path)
    {
      using namespace xercesc;
      // Create the parser
      auto parser = std::unique_ptr<SAX2XMLReader>(XMLReaderFactory::createXMLReader());

      // Define and set content and error handler
      details::ExpressionHandler<Context> handler;
      details::XMLErrorHandler error;
      parser->setContentHandler(&handler);
      parser->setErrorHandler(&error);

      // Parse
      parser->parse(path);

      // return the result
      // FIXME: return handler.getExp();
    }

  } // namespace details

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
    using namespace xercesc;
    MemBufInputSource input((XMLByte*) exp.c_str(), // The buffer location in bytes.
                            exp.size(),             // The buffer size.
                            "xml_string",           // A fake id.
                            false);                 // Xerces don't delete the buffer.
    // Create the parser
    auto parser
      = std::unique_ptr<SAX2XMLReader>(XMLReaderFactory::createXMLReader());

    // Define and set content and error handler
    details::ExpressionHandler<Context> handler;
    details::XMLErrorHandler error;
    parser->setContentHandler(&handler);
    parser->setErrorHandler(&error);

    // Parse
    parser->parse(input);

    // return the result
    // FIXME: return handler.getExp();
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
