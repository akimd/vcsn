#ifndef VCSN_CORE_RAT_VISITOR_HH_
# define VCSN_CORE_RAT_VISITOR_HH_

# include <misc/static.hh>
# include <core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet, template <class> class ConstNess>
    class gen_visitor
    {
    public:
      typedef WeightSet weightset_t;
#define VISIT(Type)                                                     \
      virtual void visit(typename ConstNess<Type<WeightSet> >::type& v)

      VISIT(concat);
      VISIT(plus);
      VISIT(kleene);
      VISIT(one);
      VISIT(zero);
      VISIT(word);
#undef VISIT
    };

    template <class WeightSet>
    struct visitor_traits
    {
      typedef gen_visitor<WeightSet, misc::id_traits> visitor;
      typedef gen_visitor<WeightSet, misc::constify_traits> const_visitor;
    };

  } // namespace rat
} // namespace vcsn

# include <core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH_
