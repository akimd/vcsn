#ifndef VCSN_CORE_RAT_LESS_THAN_HH
# define VCSN_CORE_RAT_LESS_THAN_HH

# include <vcsn/misc/cast.hh>

# include <vcsn/core/rat/fwd.hh>

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
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using ratexp_t = typename context_t::ratexp_t;
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

      bool
      operator()(ratexp_t lhs, ratexp_t rhs)
      {
        if (lhs->type() < rhs->type())
          return true;
        if (lhs->type() > rhs->type())
          return false;
        rhs_ = rhs;
        lhs->accept(*this);
        return res_;
      }

      bool
      less_than_weights(const inner_t& lhs, const inner_t& rhs)
      {
        if (weightset_t::less_than(lhs.left_weight(), rhs.left_weight()))
          return true;
        else if (weightset_t::less_than(rhs.left_weight(), lhs.left_weight()))
          return false;
        else if (weightset_t::less_than(lhs.right_weight(), rhs.right_weight()))
          return true;
        else if (weightset_t::less_than(rhs.right_weight(), lhs.right_weight()))
          return false;
        return false;
      }

      bool
      less_than_weights(const leaf_t& lhs, const leaf_t& rhs)
      {
        return weightset_t::less_than(lhs.left_weight(), rhs.left_weight());
      }


      /*-------------------------------------------------------.
      | Binary functions that compare two nodes of same type.  |
      `-------------------------------------------------------*/

      bool visit(const zero_t& lhs, const zero_t& rhs)
      {
        return less_than_weights(lhs, rhs);
      }

      bool visit(const one_t& lhs, const one_t& rhs)
      {
        return less_than_weights(lhs, rhs);
      }

      bool visit(const atom_t& lhs, const atom_t& rhs)
      {
        if (lhs.value() < rhs.value())
          return true;
        else if (rhs.value() < lhs.value())
          return false;
        else
          return less_than_weights(lhs, rhs);
      }

      // sum and prod are handled equally, as naries.
      bool visit(const nary_t& lhs, const nary_t& rhs)
      {
        auto ls = lhs.size();
        auto rs = rhs.size();
        if (ls < rs)
          return true;
        else if (rs < ls)
          return false;
        else
          for (int i = 0; i < ls; ++i)
            if (ratexpset_t::less_than(lhs[i], rhs[i]))
              return true;
            else if (ratexpset_t::less_than(rhs[i], lhs[i]))
              return false;
        return less_than_weights(lhs, rhs);
      }

      bool visit(const star_t& lhs, const star_t& rhs)
      {
        if (ratexpset_t::less_than(lhs.sub(), rhs.sub()))
          return true;
        else if (ratexpset_t::less_than(rhs.sub(), lhs.sub()))
          return false;
        return less_than_weights(lhs, rhs);
      }

      /*-----------------------------------------------------------.
      | Unary visit functions than bounces to their binary peers.  |
      `-----------------------------------------------------------*/

      virtual void
      visit(const zero_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const zero_t>(rhs_));
      }

      virtual void
      visit(const one_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const one_t>(rhs_));
      }

      virtual void
      visit(const atom_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const atom_t>(rhs_));
      }

      // sum and prod are handled equally, as naries.
      virtual void
      visit(const sum_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const nary_t>(rhs_));
      }

      // sum and prod are handled equally, as naries.
      virtual void
      visit(const prod_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const nary_t>(rhs_));
      }

      virtual void
      visit(const star_t& lhs)
      {
        res_ = visit(lhs, *down_pointer_cast<const star_t>(rhs_));
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
