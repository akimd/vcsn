#pragma once

namespace vcsn
{
  namespace rat
  {

#define DEFINE                                  \
    template <typename ExpSet>                  \
    inline                                      \
    auto                                        \
    info<ExpSet>

    DEFINE::operator()(const node_t& v)
      -> void
    {
      atom = 0;
      complement = 0;
      conjunction = 0;
      lweight = 0;
      ldiv = 0;
      one = 0;
      prod = 0;
      rweight = 0;
      shuffle = 0;
      star = 0;
      sum = 0;
      transposition = 0;
      zero = 0;
      v.accept(*this);
    }

#define VISIT(Type)                            \
    DEFINE::visit(const Type ## _t& v)         \
    -> void


    VISIT(zero)
    {
      (void) v;
      ++zero;
    }

    VISIT(one)
    {
      (void) v;
      ++one;
    }

    VISIT(atom)
    {
      (void) v;
      ++atom;
    }

    VISIT(lweight)
    {
      ++lweight;
      v.sub()->accept(*this);
    }

    VISIT(rweight)
    {
      ++rweight;
      v.sub()->accept(*this);
    }

#undef VISIT
#undef DEFINE

  } // namespace rat
} // namespace vcsn
