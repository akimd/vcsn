#ifndef VCSN_ALGOS_XML_WEIGHT_VISITOR_HH
# define VCSN_ALGOS_XML_WEIGHT_VISITOR_HH

# include <string>

# include <xercesc/dom/DOM.hpp>

namespace vcsn
{
  namespace detail
  {
    template <typename WeightSet, typename Weight>
    void
    print_weight(xercesc::DOMDocument& doc,
                 xercesc::DOMElement& root,
                 const std::string& node_name, // leftWeight or rightWeight
                 const WeightSet& ws,
                 const Weight& w);

  } // namespace detail
} // namespace vcsn

# include <vcsn/algos/xml/weight-visitor.hxx>

#endif // !VCSN_ALGOS_XML_WEIGHT_VISITOR_HH
