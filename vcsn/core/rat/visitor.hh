#ifndef VCSN_CORE_RAT_VISITOR_HH
# define VCSN_CORE_RAT_VISITOR_HH

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <class Weight>
    class const_visitor
    {
    public:
      using weight_t = Weight;
#define VISIT(Type)                                                     \
      virtual void visit(const Type<weight_t>& v)

      VISIT(zero);
      VISIT(one);
      VISIT(atom);
      VISIT(sum);
      VISIT(prod);
      VISIT(star);
#undef VISIT
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/visitor.hxx>

#endif // !VCSN_CORE_RAT_VISITOR_HH
