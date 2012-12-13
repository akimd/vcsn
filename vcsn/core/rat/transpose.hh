#ifndef VCSN_CORE_RAT_TRANSPOSE_HH
# define VCSN_CORE_RAT_TRANSPOSE_HH

# include <vcsn/core/rat/fwd.hh>

namespace vcsn
{

  namespace details
  {

    /*-----------------------.
    | transposing ratexps.  |
    `-----------------------*/

    /// \param Context  relative to the RatExp.
    template <class Context>
    class transposer
      : public Context::const_visitor
    {
    public:
      using context_t = Context;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using ratexp_t = typename context_t::ratexp_t;
      using super_type = typename Context::const_visitor;
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

      transposer(const context_t& ctx)
        : ks_{ctx}
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
        res_ = ks_.weight(ks_.weightset()->transpose(e.right_weight()),
                          res_);
        res_ = ks_.weight(res_,
                          ks_.weightset()->transpose(e.left_weight()));
      }

      void
      apply_weights(const leaf_t& e)
      {
        res_ = ks_.weight(ks_.weightset()->transpose(e.left_weight()),
                          res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ks_.zero();
      }

      virtual void
      visit(const one_t& e)
      {
        res_ = ks_.unit();
        apply_weights(e);
      }

      virtual void
      visit(const atom_t& e)
      {
        res_ = ks_.atom(ks_.genset()->transpose(e.value()));
        apply_weights(e);
      }

      virtual void
      visit(const sum_t& e)
      {
        ratexp_t res = ks_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            // There is no point in reversing, but neither not to.
            res = ks_.add(res_, res);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const prod_t& e)
      {
        ratexp_t res = ks_.unit();
        for (auto v: e)
          {
            v->accept(*this);
            res = ks_.mul(res_, res);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        res_ = ks_.star(res_);
        apply_weights(e);
      }

    private:
      typename context_t::ratexpset_t ks_;
      ratexp_t res_;
    };
  }

  /// \param Aut      relative to the generated automaton.
  /// \param Context  relative to the RatExp.
  template <class Context>
  typename Context::ratexp_t
  transpose(const Context& ctx, const typename Context::ratexp_t& v)
  {
    return ctx.make_ratexpset().transpose(v);
  }

}

#endif // !VCSN_CORE_RAT_TRANSPOSE_HH
