#ifndef VCSN_ALGOS_XML_STRUCTURE_HH
# define VCSN_ALGOS_XML_STRUCTURE_HH

# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{
  namespace detail
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
    print_gen_set(const Context&,
                  xercesc::DOMDocument&, xercesc::DOMElement*,
                  std::true_type)
    {}

    template <typename Context>
    void
    print_gen_set(const Context& ctx,
                  xercesc::DOMDocument& doc, xercesc::DOMElement* value_set,
                  std::false_type)
    {
      for (auto l: *ctx.labelset())
        {
          auto value_elt = detail::create_node(doc, "valueSetElt");
          detail::set_attribute(value_elt, "value", {l});
          value_set->appendChild(value_elt);
        }
    }

    template <typename Context>
    void
    print_label_set(const Context& ctx,
                    xercesc::DOMDocument& doc,
                    xercesc::DOMElement& root)
    {
      // Create the generator set node and sub-node
      auto value_set = detail::create_node(doc, "valueSet");

      detail::set_attribute(value_set, "genDescript", "enum");
      detail::set_attribute(value_set, "genKind", "digit");
      detail::set_attribute(value_set, "genSort", "simple");
      detail::set_attribute(value_set, "type", "free");

      print_gen_set(ctx, doc, value_set,
                    typename std::is_same<typename Context::kind_t, labels_are_one>::type());
      root.appendChild(value_set);
    }

    template <typename LabelSet,
              typename SubContext,
              template <typename, typename> class Context>
    void
    print_weight_set(const Context<LabelSet, vcsn::ratexpset<SubContext>>& ctx,
                     xercesc::DOMDocument& doc,
                     xercesc::DOMElement& root)
    {
      auto weight_set = detail::create_node(doc, "weightSet");

      detail::set_attribute(weight_set, "type", "series");
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
      auto weight_set = detail::create_node(doc, "weightSet");

      detail::set_attribute(weight_set, "type", "numerical");
      detail::set_attribute(weight_set, "set", ctx.weightset()->sname());
      detail::set_attribute(weight_set, "operations", "classical");

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

  } // namespace detail
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_STRUCTURE_HH
