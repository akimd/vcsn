#ifndef VCSN_ALGOS_XML_STRUCTURE_HH
# define VCSN_ALGOS_XML_STRUCTURE_HH

# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{
  namespace details
  {

    // Prototype

    template <typename Context>
    void
    print_label_set(const Context& ctx,
                    xercesc::DOMDocument& doc,
                    xercesc::DOMElement& root);

    template <typename Context>
    void
    print_weight_set(const Context& s,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root);

    template <typename Structure>
    void
    print_value_type(const Structure& s,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root);

    // Specialization

    template <typename Context>
    void
    print_label_set(const Context& ctx,
                    xercesc::DOMDocument& doc,
                    xercesc::DOMElement& root)
    {
      // Create the generator set node and sub-node
      auto value_set = details::create_node(doc, "valueSet");

      details::set_attribute(value_set, "genDescript", "enum");
      details::set_attribute(value_set, "genKind", "digit");
      details::set_attribute(value_set, "genSort", "simple");
      details::set_attribute(value_set, "type", "free");

      for (auto l: *ctx.labelset())
        {
          auto value_elt = details::create_node(doc, "valueSetElt");
          details::set_attribute(value_elt, "value", {l});
          value_set->appendChild(value_elt);
        }

      root.appendChild(value_set);

    }

    template <typename LabelSet,
              typename SubContext,
              typename Kind,
              template <typename, typename, typename> class Context>
    void
    print_weight_set(const Context<LabelSet,
                                   vcsn::ratexpset<SubContext>,
                                   Kind>& ctx,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root)
    {
      auto weight_set = details::create_node(doc, "weightSet");

      details::set_attribute(weight_set, "type", "series");
      print_weight_set(ctx.weightset()->context(), doc, *weight_set);
      print_label_set(ctx, doc, *weight_set);

      root.appendChild(weight_set);
    }

    template <typename Context>
    void
    print_weight_set(const Context& ctx,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root)
    {
      auto weight_set = details::create_node(doc, "weightSet");

      details::set_attribute(weight_set, "type", "numerical");
      details::set_attribute(weight_set, "set", ctx.weightset()->sname());
      details::set_attribute(weight_set, "operations", "classical");

      root.appendChild(weight_set);
    }

    template <class Context>
    void
    print_value_type(const Context& ctx,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root)
    {
      print_weight_set(ctx, doc, root);
      print_label_set(ctx, doc, root);
    }

  } // namespace details
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_STRUCTURE_HH
