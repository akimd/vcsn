#ifndef VCSN_CORE_RAT_VISITOR_HH_
# define VCSN_CORE_RAT_VISITOR_HH_

# include <vcsn/misc/const_traits.hh>
# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <class Weight, template <class> class ConstNess>
    class gen_visitor
    {
    public:
      typedef Weight weight_t;
#define VISIT(Type)                                                     \
      virtual void visit(typename ConstNess<Type<weight_t>>::type& v)

      VISIT(prod);
      VISIT(sum);
      VISIT(star);
      VISIT(one);
      VISIT(zero);
      VISIT(atom);
#undef VISIT
    };

    template <class Weight>
    struct visitor_traits
    {
      typedef gen_visitor<Weight, misc::id_traits> visitor;
      typedef gen_visitor<Weight, misc::constify_traits> const_visitor;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH_
