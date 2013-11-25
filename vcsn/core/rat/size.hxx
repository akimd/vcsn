#ifndef VCSN_CORE_RAT_SIZE_HXX
# define VCSN_CORE_RAT_SIZE_HXX

namespace vcsn
{
  namespace rat
  {

# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    size<RatExpSet>

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
      ++ size_;
      v.sub()->accept(*this);
    }

    VISIT(complement)
    {
      ++ size_;
      v.sub()->accept(*this);
    }

    VISIT(zero)
    {
      (void) v;
      ++ size_;
    }

    VISIT(one)
    {
      (void) v;
      ++ size_;
    }

    VISIT(atom)
    {
      (void) v;
      // FIXME: use the label size instead of 1.  Labels don't have a size yet.
      ++ size_;
    }

    VISIT(lweight)
    {
      ++ size_;
      v.sub()->accept(*this);
    }

    VISIT(rweight)
    {
      ++ size_;
      v.sub()->accept(*this);
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    void
    size<RatExpSet>::visit_nary(const nary_t<Type>& n)
    {
      /* An n-ary node contributes n-1 unit (plus the sum of its
         children sizes) to the ratexp size. */
      -- size_;
      for (auto child : n)
        {
          ++ size_;
          child->accept(*this);
        }
    }

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_SIZE_HXX
