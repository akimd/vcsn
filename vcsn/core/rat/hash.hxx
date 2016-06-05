#include <vcsn/misc/functional.hh>

namespace vcsn
{
  namespace rat
  {

#define DEFINE                                  \
    template <typename ExpSet>                  \
    auto                                        \
    hash<ExpSet>

#define VISIT(Type)                           \
    DEFINE::visit(const Type ## _t& v)        \
      -> void


    VISIT(atom)
    {
      visit_nullary(v);
      hash_combine(res_, ExpSet::labelset_t::hash(v.value()));
    }

    VISIT(lweight)
    {
      visit_weight_node(v);
    }

    VISIT(rweight)
    {
      visit_weight_node(v);
    }

    template <typename ExpSet>
    void
    hash<ExpSet>::combine_type(const node_t& node)
    {
      hash_combine(res_, int(node.type()));
    }

    template <typename ExpSet>
    void
    hash<ExpSet>::visit_nullary(const node_t &n)
    {
      combine_type(n);
    }

    template <typename ExpSet>
    template <type_t Type>
    void
    hash<ExpSet>::visit_unary(const unary_t<Type>& n)
    {
      combine_type(n);
      n.sub()->accept(*this);
    }

    template <typename ExpSet>
    template <type_t Type>
    void
    hash<ExpSet>::visit_weight_node(const weight_node_t<Type>& n)
    {
      combine_type(n);
      hash_combine(res_, ExpSet::weightset_t::hash(n.weight()));
      n.sub()->accept(*this);
    }

    template <typename ExpSet>
    template <type_t Type>
    void
    hash<ExpSet>::visit_variadic(const variadic_t<Type>& n)
    {
      combine_type(n);
      for (const auto& child : n)
        child->accept(*this);
    }
#undef VISIT
#undef DEFINE

  } // namespace rat
} // namespace vcsn
