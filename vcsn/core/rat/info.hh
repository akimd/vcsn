#pragma once

#include <iostream>
#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename ExpSet>
    class info
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using node_t = typename expressionset_t::node_t;
      using super_t = typename expressionset_t::const_visitor;

      /// For each node type, count its number of occurrences.
      void operator()(const node_t& v);

#define DEFINE(Type)                                    \
    public:                                             \
      using Type ## _t = typename super_t::Type ## _t;  \
      size_t Type = 0;                                  \
                                                        \
    private:                                            \
      void visit(const Type ## _t& v)

      DEFINE(atom);
      DEFINE(complement);
      DEFINE(conjunction);
      DEFINE(ldiv);
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod);
      DEFINE(rweight);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(sum);
      DEFINE(transposition);
      DEFINE(zero);

#undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/info.hxx>
