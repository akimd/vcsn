#ifndef VCSN_CORE_RAT_VISITOR_HH
# define VCSN_CORE_RAT_VISITOR_HH

# include <vcsn/core/rat/fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename Label, typename Weight>
    class const_visitor
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using node_t = node<label_t, weight_t>;
      using inner_t = inner<label_t, weight_t>;
      using leaf_t = leaf<label_t, weight_t>;
      template <type_t Type>
      using nary_t = rat::nary<Type, label_t, weight_t>;
      template <type_t Type>
      using weight_node_t = rat::weight_node<Type, label_t, weight_t>;

# define DEFINE(Type)                                   \
      using Type ## _t = Type<label_t, weight_t>;       \
      virtual void visit(const Type ## _t& v)

      DEFINE(zero);
      DEFINE(one);
      DEFINE(atom);
      DEFINE(sum);
      DEFINE(shuffle);
      DEFINE(prod);
      DEFINE(intersection);
      DEFINE(star);
      DEFINE(lweight);
      DEFINE(rweight);
# undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH
