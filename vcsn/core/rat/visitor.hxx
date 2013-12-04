#include <vcsn/core/rat/visitor.hh>
#include <vcsn/core/rat/ratexp.hh>
#include <cassert>

namespace vcsn
{
  namespace rat
  {
#define VISIT(Type, Name)                                               \
    template <typename Label, typename Weight>                          \
    inline                                                              \
    void                                                                \
    const_visitor<Label, Weight>::visit(const Type<Label, Weight>& Name)

    VISIT(prod, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(intersection, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(shuffle, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(sum, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(star, v)
    {
      if (auto sub = v.sub())
        sub->accept(*this);
    }

    VISIT(one, )
    {}

    VISIT(zero, )
    {}

    VISIT(atom, )
    {}

#undef VISIT

  } // namespace rat
} // namespace vcsn
