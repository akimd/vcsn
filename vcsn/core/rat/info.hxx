#ifndef VCSN_CORE_RAT_INFO_HXX
# define VCSN_CORE_RAT_INFO_HXX

namespace vcsn
{
  namespace rat
  {

# define DEFINE(Type)                           \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    info<RatExpSet>::visit(const Type ## _t& v) \
      -> void

    DEFINE(sum)
    {
      ++sum;
      for (const auto& c: v)
        c->accept(*this);
    }

    DEFINE(prod)
    {
      ++prod;
      for (const auto& c: v)
        c->accept(*this);
    }

    DEFINE(intersection)
    {
      ++intersection;
      for (const auto& c: v)
        c->accept(*this);
    }

    DEFINE(shuffle)
    {
      ++shuffle;
      for (const auto& c: v)
        c->accept(*this);
    }

    DEFINE(star)
    {
      ++star;
      v.sub()->accept(*this);
    }

    DEFINE(zero)
    {
      (void) v;
      ++zero;
    }

    DEFINE(one)
    {
      (void) v;
      ++one;
    }

    DEFINE(atom)
    {
      (void) v;
      ++atom;
    }

    DEFINE(lweight)
    {
      ++lweight;
      v.sub()->accept(*this);
    }

    DEFINE(rweight)
    {
      ++rweight;
      v.sub()->accept(*this);
    }

# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_INFO_HXX
