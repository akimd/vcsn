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
      auto semiring = details::create_node(doc, "semiring");

      details::set_attribute(semiring, "type", "numerical");
      details::set_attribute(semiring, "set", ctx.weightset()->sname());
      details::set_attribute(semiring, "operations", "classical");

      root.appendChild(semiring);

      // Create the generator set node and sub-node
      auto monoid = details::create_node(doc, "monoid");

      details::set_attribute(monoid, "genDescript", "enum");
      details::set_attribute(monoid, "genKind", "digit");
      details::set_attribute(monoid, "genSort", "simple");
      details::set_attribute(monoid, "type", "free");

      root.appendChild(monoid);

    }


  } // namespace details
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_STRUCTURE_HH
