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
      DEFINE(genset);
      DEFINE(letterset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(other);
      DEFINE(polynomialset);
      DEFINE(expressionset);
      DEFINE(tupleset);
      DEFINE(weightset);
      DEFINE(wordset);
# undef DEFINE
    };
  }
}

#endif // !VCSN_DYN_CONTEXT_VISITOR_HH
