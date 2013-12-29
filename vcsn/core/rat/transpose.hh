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

    /// \tparam RatExpSet  the ratexp set.
    template <class RatExpSet>
    class transposer
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
      using prod_t = typename super_type::prod_t;
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;
      using lweight_t = typename super_type::lweight_t;
      using rweight_t = typename super_type::rweight_t;

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

      virtual void
      visit(const zero_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = rs_.one();
      }

      virtual void
      visit(const atom_t& e)
      {
        res_ = rs_.atom(rs_.labelset()->transpose(e.value()));
      }

      virtual void
      visit(const sum_t& e)
      {
        ratexp_t res = rs_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            res = rs_.add(res, res_);
          }
        res_ = res;
      }

      virtual void
      visit(const intersection_t& e)
      {
        e.head()->accept(*this);
        auto res = res_;
        for (auto v: e.tail())
          {
            v->accept(*this);
            res = rs_.intersection(res, res_);
          }
        res_ = res;
      }

      virtual void
      visit(const shuffle_t& e)
      {
        // FIXME: that should be easy to factor.
        e.head()->accept(*this);
        auto res = res_;
        for (auto v: e.tail())
          {
            v->accept(*this);
            res = rs_.shuffle(res, res_);
          }
        res_ = res;
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
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        res_ = rs_.star(res_);
      }

      virtual void
      visit(const lweight_t& e)
      {
        e.sub()->accept(*this);
        res_ = rs_.rmul(res_,
                        rs_.weightset()->transpose(e.weight()));
      }

      virtual void
      visit(const rweight_t& e)
      {
        e.sub()->accept(*this);
        res_ = rs_.lmul(rs_.weightset()->transpose(e.weight()),
                        res_);
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
