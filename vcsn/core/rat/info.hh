#ifndef VCSN_CORE_RAT_INFO_HH
# define VCSN_CORE_RAT_INFO_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename RatExpSet>
    class info
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using node_t = typename ratexpset_t::node_t;
      using super_type = typename ratexpset_t::const_visitor;

      /// For each node type, count its number of occurrences.
      void operator()(const node_t& v);

# define DEFINE(Type)                                           \
    public:                                                     \
      using Type ## _t = typename super_type::Type ## _t;       \
      size_t Type = 0;                                          \
                                                                \
    private:                                                    \
      void visit(const Type ## _t& v);

      DEFINE(atom);
      DEFINE(complement);
      DEFINE(intersection);
      DEFINE(ldiv);
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod);
      DEFINE(rweight);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(sum);
      DEFINE(transposition);
      DEFINE(zero);

# undef DEFINE
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/info.hxx>

#endif // !VCSN_CORE_RAT_INFO_HH
