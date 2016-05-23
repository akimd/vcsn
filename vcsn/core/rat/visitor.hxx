#include <vcsn/core/rat/visitor.hh>
#include <vcsn/core/rat/expression.hh>
#include <cassert>

namespace vcsn
{
  namespace rat
  {
#define VISIT(Type, Name)                                       \
    template <typename Context>                                 \
    void                                                        \
    const_visitor<Context>::visit(const Type ## _t& Name)

    VISIT(compose, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(conjunction, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(infiltration, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(prod, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(shuffle, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(ldiv, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(sum, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(star, v)
    {
      v.sub()->accept(*this);
    }

    VISIT(lweight, v)
    {
      v.sub()->accept(*this);
    }

    VISIT(rweight, v)
    {
      v.sub()->accept(*this);
    }

    VISIT(complement, v)
    {
      v.sub()->accept(*this);
    }

    VISIT(transposition, v)
    {
      v.sub()->accept(*this);
    }

    VISIT(one,)
    {}

    VISIT(zero,)
    {}

    VISIT(atom,)
    {}

#undef VISIT

  } // namespace rat
} // namespace vcsn
