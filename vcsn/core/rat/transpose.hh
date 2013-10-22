#ifndef VCSN_CORE_RAT_TRANSPOSE_HH
# define VCSN_CORE_RAT_TRANSPOSE_HH

# include <vcsn/core/rat/fwd.hh>

namespace vcsn
{

  namespace detail
  {

    /*----------------------.
    | transposing ratexps.  |
    `----------------------*/

    /// \param Context  relative to the RatExp.
    template <class RatExpSet>
    class LIBVCSN_API transposer
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

      transposer(const ratexpset_t& rs)
        : rs_{rs}
        , res_{}
      {}

      ratexp_t
      operator()(const ratexp_t& e)
      {
        e->accept(*this);
        return std::move(res_);
      }

      void
      apply_weights(const inner_t& e)
      {
        // Apply left on right, and vice-versa.  Transposed.
        res_ = rs_.weight(rs_.weightset()->transpose(e.right_weight()),
                          res_);
        res_ = rs_.weight(res_,
                          rs_.weightset()->transpose(e.left_weight()));
      }

      void
      apply_weights(const leaf_t& e)
      {
        res_ = rs_.weight(rs_.weightset()->transpose(e.left_weight()),
                          res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const one_t& e)
      {
        res_ = rs_.one();
        apply_weights(e);
      }

      virtual void
      visit(const atom_t& e)
      {
        res_ = rs_.atom(rs_.labelset()->transpose(e.value()));
        apply_weights(e);
      }

      virtual void
      visit(const sum_t& e)
      {
        ratexp_t res = rs_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            // There is no point in reversing, but neither not to.
            res = rs_.add(res_, res);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const prod_t& e)
      {
        ratexp_t res = rs_.one();
        for (auto v: e)
          {
            v->accept(*this);
            res = rs_.mul(res_, res);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        res_ = rs_.star(res_);
        apply_weights(e);
      }

    private:
      ratexpset_t rs_;
      ratexp_t res_;
    };
  }

  template <class RatExpSet>
  typename RatExpSet::ratexp_t
  transpose(const RatExpSet& rs, const typename RatExpSet::ratexp_t& v)
  {
    return rs.transpose(v);
  }

}

#endif // !VCSN_CORE_RAT_TRANSPOSE_HH
