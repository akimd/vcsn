#ifndef VCSN_CORE_RAT_SIZE_HH
# define VCSN_CORE_RAT_SIZE_HH

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {
    template <typename RatExpSet>
    class size
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      template <type_t Type>
      using nary_t = typename super_type::template nary_t<Type>;
      using leaf_t = typename super_type::leaf_t;

      /// Entry point: return the size of \a v.
      size_t
      operator()(const node_t& v)
      {
        size_ = 0;
        v.accept(*this);
        return size_;
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

      size_t size_;
    };
  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/size.hxx>

#endif // !VCSN_CORE_RAT_SIZE_HH
