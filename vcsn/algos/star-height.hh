#ifndef VCSN_ALGOS_STAR_HEIGHT_HH
# define VCSN_ALGOS_STAR_HEIGHT_HH

# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename RatExpSet>
    class star_heighter
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      template <rat::type_t Type>
      using nary_t = typename super_type::template nary_t<Type>;

      /// Entry point: return the size of \a v.
      unsigned
      operator()(const node_t& v)
      {
        height_ = 0;
        v.accept(*this);
        return height_;
      }

      /// Entry point: return the size of \a v.
      unsigned
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

# define DEFINE(Type)						\
      using Type ## _t = typename super_type::Type ## _t;	\
      virtual void visit(const Type ## _t& v)

      DEFINE(atom)         { (void) v; }
      DEFINE(complement)   { v.sub()->accept(*this); }
      DEFINE(intersection) { visit_nary(v); }
      DEFINE(ldiv)         { visit_nary(v); }
      DEFINE(lweight)      { v.sub()->accept(*this); }
      DEFINE(one)          { (void) v; }
      DEFINE(prod)         { visit_nary(v); }
      DEFINE(rweight)      { v.sub()->accept(*this); }
      DEFINE(shuffle)      { visit_nary(v); }
      DEFINE(star)         { ++height_; v.sub()->accept(*this); }
      DEFINE(sum)          { visit_nary(v); }
      DEFINE(zero)         { (void) v; }

# undef DEFINE

      /// Traverse variadic node.
      template <rat::type_t Type>
      void
      visit_nary(const nary_t<Type>& n)
      {
        /* The height of an n-ary is the max of the n heights. */
        auto max = height_;
        auto initial = height_;
        for (auto child : n)
          {
            height_ = initial;
            child->accept(*this);
            if (max < height_)
              max = height_;
          }
        height_ = max;
      }

      unsigned height_;
    };
  } // namespace detail


  /// Star height of a ratexp.
  template <typename RatExpSet>
  inline
  unsigned
  star_height(const typename RatExpSet::ratexp_t& e)
  {
    detail::star_heighter<RatExpSet> s;
    return s(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      unsigned
      star_height(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return ::vcsn::star_height<RatExpSet>(e.ratexp());
      }

      REGISTER_DECLARE(star_height,
                       (const ratexp& e) -> unsigned);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_STAR_HEIGHT_HH
