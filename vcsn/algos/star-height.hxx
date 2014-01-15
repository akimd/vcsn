#ifndef VCSN_ALGOS_STAR_HEIGHT_HXX
# define VCSN_ALGOS_STAR_HEIGHT_HXX

namespace vcsn
{
  namespace detail
  {

# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    star_heighter<RatExpSet>

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void

    VISIT(sum)
    {
      visit_nary(v);
    }

    VISIT(prod)
    {
      visit_nary(v);
    }

    VISIT(shuffle)
    {
      visit_nary(v);
    }

    VISIT(intersection)
    {
      visit_nary(v);
    }

    VISIT(star)
    {
      ++height_;
      v.sub()->accept(*this);
    }

    VISIT(complement)
    {
      v.sub()->accept(*this);
    }

    VISIT(zero)
    {
      (void) v;
    }

    VISIT(one)
    {
      (void) v;
    }

    VISIT(atom)
    {
      (void) v;
    }

    VISIT(lweight)
    {
      v.sub()->accept(*this);
    }

    VISIT(rweight)
    {
      v.sub()->accept(*this);
    }

    template <typename RatExpSet>
    template <rat::type_t Type>
    inline
    void
    star_heighter<RatExpSet>::visit_nary(const nary_t<Type>& n)
    {
      /* The height of an n-ary is the max of the n heights. */
      size_t max = height_;
      size_t initial = height_;
      for (auto child : n)
        {
          height_ = initial;
          child->accept(*this);
          if (max < height_)
            max = height_;
        }
      height_ = max;
    }

# undef VISIT
# undef DEFINE

  } // namespace detail
} // namespace vcsn

#endif // !VCSN_ALGOS_STAR_HEIGHT_HXX
