#include <cassert>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
#define VISIT(Type, Name)                                       \
    template <typename Context>                                 \
    void                                                        \
    const_visitor<Context>::visit(const Type ## _t& Name)

    VISIT(add, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

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

    VISIT(infiltrate, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(mul, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(shuffle, v)
    {
      for (const auto& t : v)
        t->accept(*this);
    }

    VISIT(ldivide, v)
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
