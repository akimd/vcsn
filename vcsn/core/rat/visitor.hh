#ifndef VCSN_CORE_RAT_VISITOR_HH
# define VCSN_CORE_RAT_VISITOR_HH

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
      using weight_t = Weight;
#define VISIT(Type)                                                     \
      virtual void visit(typename ConstNess<Type<weight_t>>::type& v)

      VISIT(zero);
      VISIT(one);
      VISIT(atom);
      VISIT(sum);
      VISIT(prod);
      VISIT(star);
#undef VISIT
    };

    template <class Weight>
    struct visitor_traits
    {
      using visitor = gen_visitor<Weight, misc::id_traits>;
      using const_visitor = gen_visitor<Weight, misc::constify_traits>;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH
