#ifndef VCSN_ALGOS_XML_STRUCTURE_HH
# define VCSN_ALGOS_XML_STRUCTURE_HH

# include <vcsn/core/rat/kratexp.hh>

namespace vcsn
{
  namespace details
  {

    template <typename Structure>
    void
    print_value_type(Structure& s,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root);

    template <typename Context>
    void
    print_value_type(const Context& ctx, xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root)
    {
      // Create the weightset node and sub-node
      auto weight_set = details::create_node(doc, "weightSet");

      details::set_attribute(weight_set, "type", "numerical");
      details::set_attribute(weight_set, "set", ctx.weightset()->sname());
      details::set_attribute(weight_set, "operations", "classical");

      root.appendChild(weight_set);

      // Create the generator set node and sub-node
      auto value_set = details::create_node(doc, "valueSet");

      details::set_attribute(value_set, "genDescript", "enum");
      details::set_attribute(value_set, "genKind", "digit");
      details::set_attribute(value_set, "genSort", "simple");
      details::set_attribute(value_set, "type", "free");

      for (auto l: *ctx.genset())
        {
          auto value_elt = details::create_node(doc, "valueSetElt");
          details::set_attribute(value_elt, "value", {l});
          value_set->appendChild(value_elt);
        }


      root.appendChild(value_set);

    }


  } // namespace details
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_STRUCTURE_HH
