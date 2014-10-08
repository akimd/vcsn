#ifndef VCSN_CORE_RAT_VISITOR_HH
# define VCSN_CORE_RAT_VISITOR_HH

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename Context>
    class const_visitor
    {
    public:
      using context_t = Context;
      using node_t = node<context_t>;
      using inner_t = inner<context_t>;
      using leaf_t = leaf<context_t>;
      template <type_t Type>
      using unary_t = rat::unary<Type, context_t>;
      template <type_t Type>
      using variadic_t = rat::variadic<Type, context_t>;
      template <type_t Type>
      using weight_node_t = rat::weight_node<Type, context_t>;

# define DEFINE(Type)                           \
      using Type ## _t = Type<context_t>;       \
      virtual void visit(const Type ## _t& v)

      DEFINE(atom);
      DEFINE(complement);
      DEFINE(conjunction);
      DEFINE(ldiv);
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod);
      DEFINE(rweight);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(sum);
      DEFINE(transposition);
      DEFINE(zero);
# undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

# define VCSN_RAT_VISIT(Type, Val)                      \
  using Type ## _t = typename super_t::Type ## _t;      \
  virtual void visit(const Type ## _t& Val)

# define VCSN_RAT_UNSUPPORTED(Type)                     \
  VCSN_RAT_VISIT(Type,)                                 \
  {                                                     \
    raise(me(), ": " #Type " is not supported");        \
  }

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH
