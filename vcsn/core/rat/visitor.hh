#ifndef VCSN_CORE_RAT_VISITOR_HH
# define VCSN_CORE_RAT_VISITOR_HH

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename Atom, typename Weight>
    class const_visitor
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using node_t = node<atom_value_t, weight_t>;
      using inner_t = inner<atom_value_t, weight_t>;
      using nary_t = nary<atom_value_t, weight_t>;
      using leaf_t = leaf<atom_value_t, weight_t>;
# define DEFINE(Type)                                         \
      using Type ## _t = Type<atom_value_t, weight_t>;        \
      virtual void visit(const Type ## _t& v)

      DEFINE(zero);
      DEFINE(one);
      DEFINE(atom);
      DEFINE(sum);
      DEFINE(prod);
      DEFINE(star);
# undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH
