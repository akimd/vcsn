#ifndef VCSN_DYN_CONTEXT_VISITOR_HH
# define VCSN_DYN_CONTEXT_VISITOR_HH

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace ast
  {

    class context_visitor
    {
    public:
# define DEFINE(Type)                   \
      virtual void visit(const Type& t) = 0

      DEFINE(automaton);
      DEFINE(context);
      DEFINE(tupleset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(letterset);
      DEFINE(ratexpset);
      DEFINE(weightset);
      DEFINE(wordset);
      DEFINE(other);
      DEFINE(polynomialset);
# undef DEFINE
    };
  }
}

#endif /* !VCSN_DYN_CONTEXT_VISITOR_HH */
