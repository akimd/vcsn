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
      template <rat::exp::type_t Type>
      using weight_node_t = typename super_type::template weight_node_t<Type>;

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

      /*-----------------------------------------------------------.
      | Unary visit functions than bounces to their binary peers.  |
      `-----------------------------------------------------------*/

#define VISIT(Type)                                                     \
      using Type ## _t = typename super_type::Type ## _t;               \
      virtual void                                                      \
      visit(const Type ## _t& lhs)                                      \
      {                                                                 \
        res_ = less_than_(lhs, *down_pointer_cast<const Type ## _t>(rhs_)); \
      }

      VISIT(atom);
      VISIT(complement);
      VISIT(intersection);
      VISIT(ldiv);
      VISIT(lweight);
      VISIT(one);
      VISIT(prod);
      VISIT(rweight);
      VISIT(shuffle);
      VISIT(star);
      VISIT(sum);
      VISIT(zero);
#undef VISIT

      /*-------------------------------------------------------.
      | Binary functions that compare two nodes of same type.  |
      `-------------------------------------------------------*/

      bool less_than_(const zero_t&, const zero_t&)
      {
        return false;
      }

      bool less_than_(const one_t&, const one_t&)
      {
        return false;
      }

      bool less_than_(const atom_t& lhs, const atom_t& rhs)
      {
        return labelset_t::less_than(lhs.value(), rhs.value());
      }

      template <rat::exp::type_t Type>
      bool less_than_(const nary_t<Type>& lhs, const nary_t<Type>& rhs)
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
      bool less_than_(const unary_t<Type>& lhs, const unary_t<Type>& rhs)
      {
        return ratexpset_t::less_than(lhs.sub(), rhs.sub());
      }

      template <rat::exp::type_t Type>
      bool less_than_(const weight_node_t<Type>& lhs, const weight_node_t<Type>& rhs)
      {
        // Lexicographic comparison on sub-expression, and then weight.
        if (ratexpset_t::less_than(lhs.sub(), rhs.sub()))
          return true;
        else if (ratexpset_t::less_than(rhs.sub(), lhs.sub()))
          return false;
        else
          return weightset_t::less_than(lhs.weight(), rhs.weight());
      }

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
