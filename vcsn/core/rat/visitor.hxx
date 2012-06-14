#include <vcsn/core/rat/visitor.hh>
#include <vcsn/core/rat/kratexp.hh>
#include <cassert>

namespace vcsn
{
  namespace rat
  {
#define VISIT(Type, Name)                                               \
    template <typename Atom, typename Weight>                           \
    inline                                                              \
    void                                                                \
    const_visitor<Atom, Weight>::visit(const Type<Atom, Weight>& Name)

    VISIT(prod, v)
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
