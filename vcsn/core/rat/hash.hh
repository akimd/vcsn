#ifndef VCSN_CORE_RAT_HASH_HH
# define VCSN_CORE_RAT_HASH_HH

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {
    template <typename RatExpSet>
    class hash
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      template <type_t Type>
      using nary_t = typename super_type::template nary_t<Type>;
      template <type_t Type>
      using unary_t = typename super_type::template unary_t<Type>;
      template <type_t Type>
      using weight_node_t = typename super_type::template weight_node_t<Type>;
      using leaf_t = typename super_type::leaf_t;

      /// Entry point: return the hash of \a v.
      size_t
      operator()(const node_t& v)
      {
        res_ = 0;
        v.accept(*this);
        return res_;
      }

      /// Entry point: return the hash of \a v.
      size_t
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

# define DEFINE(Type)						\
      using Type ## _t = typename super_type::Type ## _t;	\
      virtual void visit(const Type ## _t& v)

      DEFINE(atom);
      DEFINE(complement)   { visit_unary(v); }
      DEFINE(intersection) { visit_nary(v); }
      DEFINE(ldiv)    { visit_nary(v); }
      DEFINE(lweight);
      DEFINE(one)          { visit_nullary(v); }
      DEFINE(prod)         { visit_nary(v); }
      DEFINE(rweight);
      DEFINE(shuffle)      { visit_nary(v); }
      DEFINE(star)         { visit_unary(v); }
      DEFINE(sum)          { visit_nary(v); }
      DEFINE(zero)         { visit_nullary(v); }

# undef DEFINE

      /// Update res_ by hashing the node type; this is needed for any node.
      void combine_type(const node_t& node);

      /// Traverse a nullary node (atom, \z, \e).
      void visit_nullary(const node_t& v);

      /// Traverse a unary node (*, {c}).
      template <rat::exp::type_t Type>
      void visit_unary(const unary_t<Type>& v);

      /// Traverse an n-ary node (+, ., &, :).
      template <rat::exp::type_t Type>
      void visit_nary(const nary_t<Type>& v);

      /// Traverse a weight node (lweight, rweight).
      template <rat::exp::type_t Type>
      void visit_weight_node(const weight_node_t<Type>& v);

      size_t res_;
    };
  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/hash.hxx>

#endif // !VCSN_CORE_RAT_HASH_HH
