#ifndef VCSN_CORE_RAT_INFO_HXX
# define VCSN_CORE_RAT_INFO_HXX

namespace vcsn
{
  namespace rat
  {

# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    info<RatExpSet>

    DEFINE::operator()(const node_t& v)
      -> void
    {
      atom = 0;
      intersection = 0;
      lweight = 0;
      one = 0;
      prod = 0;
      rweight = 0;
      shuffle = 0;
      star = 0;
      sum = 0;
      zero = 0;
      v.accept(*this);
    }

# define VISIT(Type)                           \
    DEFINE::visit(const Type ## _t& v)         \
    -> void

    VISIT(sum)
    {
      ++sum;
      for (const auto& c: v)
        c->accept(*this);
    }

    VISIT(prod)
    {
      ++prod;
      for (const auto& c: v)
        c->accept(*this);
    }

    VISIT(intersection)
    {
      ++intersection;
      for (const auto& c: v)
        c->accept(*this);
    }

    VISIT(shuffle)
    {
      ++shuffle;
      for (const auto& c: v)
        c->accept(*this);
    }

    VISIT(star)
    {
      ++star;
      v.sub()->accept(*this);
    }

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

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_INFO_HXX
