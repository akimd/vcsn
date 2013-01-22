#ifndef VCSN_ALGOS_XML_WEIGHT_VISITOR_HXX
# define VCSN_ALGOS_XML_WEIGHT_VISITOR_HXX

# include <memory>

# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/algos/xml/ratexpvisitor.hh>

namespace vcsn
{
  namespace details
  {
    template <typename Context, typename Weight>
    void
    print_weight(xercesc::DOMDocument& doc,
                 xercesc::DOMElement& root,
                 const std::string& name,
                 const std::shared_ptr<vcsn::ratexpset<Context> const>& ws,
                 const Weight& w)
    {
      using xml_visitor = rat::xml_ratexp_visitor<Context>;
      using node_t = typename rat::xml_ratexp_visitor<Context>::node_t;

      auto rat_exp_node = details::create_node(doc, name);
      root.appendChild(rat_exp_node);
      xml_visitor v(doc, *rat_exp_node, ws->context());
      v(static_cast<const node_t&>(*w));
    }

    template <typename WeightSet, typename Weight>
    void
    print_weight(xercesc::DOMDocument& doc,
                 xercesc::DOMElement& root,
                 const std::string& name,
                 const WeightSet& ws,
                 const Weight& w)
    {
      assert(name == "leftWeight" || name == "rightWeight");
      auto weight = details::create_node(doc, name);
      details::set_attribute(weight, "value", ws->format(w));
      root.appendChild(weight);
    }
  } // namespace details
} // namespace vcsn
#endif // !VCSN_ALGOS_XML_WEIGHT_VISITOR_HXX
