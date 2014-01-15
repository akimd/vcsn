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
      size_t
      operator()(const node_t& v)
      {
        height_ = 0;
        v.accept(*this);
        return height_;
      }

      /// Entry point: return the size of \a v.
      size_t
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

# define DEFINE(Type)						\
      using Type ## _t = typename super_type::Type ## _t;	\
      virtual void visit(const Type ## _t& v)

      DEFINE(sum);
      DEFINE(prod);
      DEFINE(shuffle);
      DEFINE(intersection);
      DEFINE(star);
      DEFINE(complement);
      DEFINE(one);
      DEFINE(zero);
      DEFINE(atom);
      DEFINE(lweight);
      DEFINE(rweight);

# undef DEFINE

      /// Traverse n-ary node (+ and .).
      template <rat::exp::type_t Type>
      void visit_nary(const nary_t<Type>& n);

      size_t height_;
    };
  } // namespace detail



  /// Star height of a ratexp.
  template <typename RatExpSet>
  inline
  size_t
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
      size_t
      star_height(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return ::vcsn::star_height<RatExpSet>(e.ratexp());
      }

      REGISTER_DECLARE(star_height,
                       (const ratexp& e) -> size_t);
    }
  }

} // namespace vcsn

# include <vcsn/algos/star-height.hxx>

#endif // !VCSN_ALGOS_STAR_HEIGHT_HH
