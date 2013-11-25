#ifndef VCSN_CORE_RAT_LESS_THAN_HH
# define VCSN_CORE_RAT_LESS_THAN_HH

# include <vcsn/misc/cast.hh>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/size.hh>

namespace vcsn
{

  namespace rat
  {

    template <class RatExpSet>
    class less_than
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using labelset_t = typename context_t::labelset_t;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using ratexp_t = typename context_t::ratexp_t;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      template <rat::exp::type_t Type>
      using unary_t = typename super_type::template unary_t<Type>;
      template <rat::exp::type_t Type>
      using nary_t = typename super_type::template nary_t<Type>;
      using prod_t = typename super_type::prod_t;
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
      using sum_t = typename super_type::sum_t;
      template <rat::exp::type_t Type>
      using weight_node_t = typename super_type::template weight_node_t<Type>;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using complement_t = typename super_type::complement_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;
      using lweight_t = typename super_type::lweight_t;
      using rweight_t = typename super_type::rweight_t;

      /// Whether \a lhs < \a rhs.
      bool
      operator()(ratexp_t lhs, ratexp_t rhs)
      {
        rat::size<RatExpSet> sizer;
        size_t lhss = sizer(lhs);
        size_t rhss = sizer(rhs);

        if (lhss < rhss)
          return true;
        else if (lhss > rhss)
          return false;
        else if (lhs->type() < rhs->type())
          return true;
        else if (lhs->type() > rhs->type())
          return false;
        else
          {
            rhs_ = rhs;
            lhs->accept(*this);
            return res_;
          }
      }


      /*-------------------------------------------------------.
      | Binary functions that compare two nodes of same type.  |
      `-------------------------------------------------------*/

      bool visit(const zero_t&, const zero_t&)
      {
        return false;
      }

      bool visit(const one_t&, const one_t&)
      {
        return false;
      }

      bool visit(const atom_t& lhs, const atom_t& rhs)
      {
        return labelset_t::less_than(lhs.value(), rhs.value());
      }

      template <rat::exp::type_t Type>
      bool visit(const nary_t<Type>& lhs, const nary_t<Type>& rhs)
      {
        auto ls = lhs.size();
        auto rs = rhs.size();
        if (ls < rs)
          return true;
        else if (rs < ls)
          return false;
        else
          for (size_t i = 0; i < ls; ++i)
            if (ratexpset_t::less_than(lhs[i], rhs[i]))
              return true;
            else if (ratexpset_t::less_than(rhs[i], lhs[i]))
              return false;
        return false;
      }

      template <rat::exp::type_t Type>
      bool visit(const unary_t<Type>& lhs, const unary_t<Type>& rhs)
      {
        return ratexpset_t::less_than(lhs.sub(), rhs.sub());
      }

      template <rat::exp::type_t Type>
      bool visit(const weight_node_t<Type>& lhs, const weight_node_t<Type>& rhs)
      {
        // Lexicographic comparison on sub-expression, and then weight.
        if (ratexpset_t::less_than(lhs.sub(), rhs.sub()))
          return true;
        else if (ratexpset_t::less_than(rhs.sub(), lhs.sub()))
          return false;
        else
          return weightset_t::less_than(lhs.weight(), rhs.weight());
      }

      /*-----------------------------------------------------------.
      | Unary visit functions than bounces to their binary peers.  |
      `-----------------------------------------------------------*/

#define VISIT(Type)                                                     \
      virtual void                                                      \
      visit(const Type ## _t& lhs)                                      \
      {                                                                 \
        res_ = visit(lhs, *down_pointer_cast<const Type ## _t>(rhs_));  \
      }

      VISIT(zero);
      VISIT(one);
      VISIT(atom);
      VISIT(prod);
      VISIT(sum);
      VISIT(shuffle);
      VISIT(intersection);
      VISIT(star);
      VISIT(lweight);
      VISIT(rweight);
      VISIT(complement);
#undef VISIT

   private:
      ratexp_t rhs_;
      bool res_;
    };
  }

  template <class RatExpSet>
  typename RatExpSet::ratexp_t
  less_than(const RatExpSet& rs, const typename RatExpSet::ratexp_t& v)
  {
    return rs.less_than(v);
  }

}

#endif // !VCSN_CORE_RAT_LESS_THAN_HH
