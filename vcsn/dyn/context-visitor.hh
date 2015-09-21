#pragma once

#include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace ast
  {

    class context_visitor
    {
    public:
#define DEFINE(Type)                            \
      virtual void visit(const Type& t) = 0

      DEFINE(automaton);
      DEFINE(context);
      DEFINE(expansionset);
      DEFINE(expressionset);
      DEFINE(genset);
      DEFINE(letterset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(other);
      DEFINE(polynomialset);
      DEFINE(tuple);
      DEFINE(tupleset);
      DEFINE(weightset);
      DEFINE(wordset);
#undef DEFINE
    };
  }
}
