#pragma once

#include <lib/vcsn/dyn/fwd.hh>

namespace vcsn::ast
{
  /// Traverse a type-ast.
  class visitor
  {
  public:
#define DEFINE(Type)                            \
    virtual void visit(const Type& t) = 0

    DEFINE(automaton);
    DEFINE(context);
    DEFINE(expansionset);
    DEFINE(expressionset);
    DEFINE(genset);
    DEFINE(genlabelset);
    DEFINE(oneset);
    DEFINE(other);
    DEFINE(polynomialset);
    DEFINE(tuple);
    DEFINE(tupleset);
    DEFINE(weightset);
#undef DEFINE
  };
}
