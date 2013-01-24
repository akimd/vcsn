#ifndef VCSN_ALGOS_XML_KRATEXPVISITOR_HH
# define VCSN_ALGOS_XML_KRATEXPVISITOR_HH

# include <memory>

# include <xercesc/dom/DOM.hpp>
# include <xercesc/util/XMLString.hpp>
# include <xercesc/util/PlatformUtils.hpp>

namespace vcsn
{
  namespace rat
  {
    template <typename Context>
    class xml_ratexp_visitor : public Context::const_visitor
    {
    public:
      using context_t  = Context;
      using weight_t   = typename context_t::weightset_t::value_t;
      using weight_ptr = std::shared_ptr<const typename context_t::weightset_t>;
      using label_t    = typename context_t::weightset_t::value_t;
      using this_t     = xml_ratexp_visitor<context_t>;
      using super_type = typename context_t::const_visitor;
      using node_t     = typename super_type::node_t;
      using node_ptr   = std::shared_ptr<const node_t>;
      using inner_t    = typename super_type::inner_t;
      using nary_t     = typename super_type::nary_t;
      using prod_t     = typename super_type::prod_t;
      using sum_t      = typename super_type::sum_t;
      using leaf_t     = typename super_type::leaf_t;
      using star_t     = typename super_type::star_t;
      using zero_t     = typename super_type::zero_t;
      using one_t      = typename super_type::one_t;
      using atom_t     = typename super_type::atom_t;
      using dom_elt_t  = xercesc::DOMElement;

      xml_ratexp_visitor(xercesc::DOMDocument& doc,
                          xercesc::DOMElement& root,
                          const context_t& ctx);
      ~xml_ratexp_visitor();

      void operator()(const node_t& v);
      void operator()(const node_ptr& v);

      virtual void visit(const zero_t& v);
      virtual void visit(const one_t& v);
      virtual void visit(const atom_t& v);
      virtual void visit(const sum_t& v);
      virtual void visit(const prod_t& v);
      virtual void visit(const star_t& v);
    private:
      bool shows_(const weight_t& w) const;
      void print_weight(const weight_t& w, dom_elt_t& root);

      xercesc::DOMDocument& doc_;
      xercesc::DOMElement& root_;
      const context_t& ctx_;
      const weight_ptr ws_;
    };
  } // namespace rat
} // namespace vcsn

# include <vcsn/algos/xml/ratexpvisitor.hxx>

#endif // !VCSN_ALGOS_XML_KRATEXPVISITOR_HH
