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

      VCSN_RAT_VISIT(zero,)
      {
        res_ = rs_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = rs_.one();
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = rs_.atom(rs_.labelset()->transpose(e.value()));
      }

      VCSN_RAT_VISIT(sum, e)
      {
        ratexp_t res = rs_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            res = rs_.add(res, res_);
          }
        res_ = res;
      }

      VCSN_RAT_VISIT(intersection, e)
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

      VCSN_RAT_VISIT(shuffle, e)
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

      VCSN_RAT_VISIT(prod, e)
      {
        ratexp_t res = rs_.one();
        for (auto v: e)
          {
            v->accept(*this);
            res = rs_.mul(res_, res);
          }
        res_ = res;
      }

      VCSN_RAT_VISIT(star, e)
      {
        e.sub()->accept(*this);
        res_ = rs_.star(res_);
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        res_ = rs_.rmul(res_,
                        rs_.weightset()->transpose(e.weight()));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);
        res_ = rs_.lmul(rs_.weightset()->transpose(e.weight()),
                        res_);
      }

      VCSN_RAT_VISIT(complement, e)
      {
        e.sub()->accept(*this);
        res_ = rs_.complement(res_);
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
