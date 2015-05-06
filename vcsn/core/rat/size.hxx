namespace vcsn
{
  namespace rat
  {

#define DEFINE                                  \
    template <typename ExpSet>                  \
    inline                                      \
    auto                                        \
    size<ExpSet>

#define VISIT(Type)                             \
    DEFINE::visit(const Type ## _t& v)          \
      -> void

    VISIT(zero)
    {
      (void) v;
      ++size_;
    }

    VISIT(one)
    {
      (void) v;
      ++size_;
    }

    VISIT(atom)
    {
      (void) v;
      // FIXME: use the label size instead of 1.  Labels don't have a size yet.
      ++size_;
    }

    VISIT(lweight)
    {
      ++size_;
      v.sub()->accept(*this);
    }

    VISIT(rweight)
    {
      ++size_;
      v.sub()->accept(*this);
    }

    template <typename ExpSet>
    template <type_t Type>
    inline
    void
    size<ExpSet>::visit_unary(const unary_t<Type>& v)
    {
      ++size_;
      v.sub()->accept(*this);
    }

    template <typename ExpSet>
    template <type_t Type>
    inline
    void
    size<ExpSet>::visit_variadic(const variadic_t<Type>& n)
    {
      // One operator bw each argument.
      size_ += n.size() - 1;
      for (auto child : n)
        child->accept(*this);
    }


#undef VISIT
#undef DEFINE

  } // namespace rat
} // namespace vcsn
