#ifndef VCSN_CORE_RAT_HASH_HXX
# define VCSN_CORE_RAT_HASH_HXX

# include <vcsn/misc/hash.hh>

namespace vcsn
{
  namespace rat
  {

# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    hash<RatExpSet>

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void


    VISIT(atom)
    {
      visit_nullary(v);
      std::hash_combine(res_, RatExpSet::labelset_t::hash(v.value()));
    }

    VISIT(lweight)
    {
      visit_weight_node(v);
    }

    VISIT(rweight)
    {
      visit_weight_node(v);
    }

    template <typename RatExpSet>
    inline
    void
    hash<RatExpSet>::combine_type(const node_t& node)
    {
      std::hash_combine(res_, int(node.type()));
    }

    template <typename RatExpSet>
    inline
    void
    hash<RatExpSet>::visit_nullary(const node_t &n)
    {
      combine_type(n);
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    void
    hash<RatExpSet>::visit_unary(const unary_t<Type>& n)
    {
      combine_type(n);
      n.sub()->accept(*this);
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    void
    hash<RatExpSet>::visit_weight_node(const weight_node_t<Type>& n)
    {
      combine_type(n);
      std::hash_combine(res_, RatExpSet::weightset_t::hash(n.weight()));
      n.sub()->accept(*this);
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    void
    hash<RatExpSet>::visit_nary(const nary_t<Type>& n)
    {
      combine_type(n);
      for (auto child : n)
        child->accept(*this);
    }
# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_HASH_HXX
