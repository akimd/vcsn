#ifndef VCSN_CORE_RAT_INFO_HH
# define VCSN_CORE_RAT_INFO_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename RatExpSet>
    class LIBVCSN_API info
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      /// Entry point.
      void
      operator()(const node_t& v)
      {
        sum = prod = star = zero = one = atom = 0;
        v.accept(*this);
      }

# define DEFINE(Type)                           \
    public:                                     \
      size_t Type = 0;                          \
                                                \
    private:                                    \
      void visit(const Type ## _t& v);

      DEFINE(sum);
      DEFINE(prod);
      DEFINE(star);
      DEFINE(zero);
      DEFINE(one);
      DEFINE(atom);

# undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/info.hxx>

#endif // !VCSN_CORE_RAT_INFO_HH
