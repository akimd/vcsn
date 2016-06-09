#pragma once

#include <vcsn/core/rat/fwd.hh>

namespace vcsn
{

  namespace detail
  {

    /*-------------------------.
    | transpose(expression).   |
    `-------------------------*/

    /// A visitor to create a transposed expression,
    ///
    /// \tparam ExpSet  the expression set.
    template <typename ExpSet>
    class transposer
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using expression_t = typename expressionset_t::value_t;
      using super_t = typename expressionset_t::const_visitor;
      using context_t = context_t_of<expressionset_t>;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "transpose"; }

      transposer(const expressionset_t& rs)
        : rs_{rs}
        , res_{}
      {}

      expression_t
      operator()(const expression_t& e)
      {
        e->accept(*this);
        return std::move(res_);
      }

      expression_t
      transpose(const expression_t& e)
      {
        expression_t res;
        std::swap(res_, res);
        e->accept(*this);
        std::swap(res_, res);
        return res;
      }

    private:
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

      /// Binary member functions of the expressionset.
      using binary_t =
        expression_t (expressionset_t::*)(const expression_t&,
                                          const expression_t&) const;

      template <rat::type_t Type>
      using variadic_t = rat::variadic<Type, context_t>;

      /// Factor the visitation of "commutative" variadic nodes.
      template <rat::type_t Type>
      void visit_(const variadic_t<Type>& v, binary_t f)
      {
        res_ = transpose(v.head());
        for (const auto& c: v.tail())
          res_ = (rs_.*f)(res_, transpose(c));
      }

      /// Handle variadic operations.
#define DEFINE(Node)                            \
      VCSN_RAT_VISIT(Node, e)                   \
      {                                         \
        visit_(e, &expressionset_t::Node);      \
      }

      DEFINE(add);
      DEFINE(conjunction);
      DEFINE(infiltrate);
      DEFINE(shuffle);
#undef DEFINE

      VCSN_RAT_VISIT(mul, e)
      {
        res_ = transpose(e.head());
        for (const auto& c: e.tail())
          res_ = rs_.mul(transpose(c), res_);
      }

      VCSN_RAT_VISIT(star, e)
      {
        res_ = rs_.star(transpose(e.sub()));
      }

      VCSN_RAT_VISIT(complement, e)
      {
        res_ = rs_.complement(transpose(e.sub()));
      }

      VCSN_RAT_VISIT(transposition, e)
      {
        // Don't stack indefinitly transpositions on top of
        // transitions.  Not only is this useless, it would also break
        // the involution as r.transpose().transpose() would not be r,
        // but "r{T}{T}".  On the other hand, if "(abc){T}".transpose()
        // returns "abc", we also lose the involution.
        //
        // So rather, don't stack more that two transpositions:
        //
        // (abc){T}.transpose() => (abc){T}{T}
        // (abc){T}{T}.transpose() => (abc){T}
        //
        // Do the same with ldiv, for the same reasons: involution.
        //
        // (E\F).transpose() => (E\F){T}
        // (E\F){T}.transpose() => (E\F)
        if (e.sub()->type() == rat::type_t::transposition
            || e.sub()->type() == rat::type_t::ldiv)
          res_ = e.sub();
        else
          res_ = rs_.transposition(e.shared_from_this());
      }

      VCSN_RAT_VISIT(ldiv, e)
      {
        // There is nothing we can do here but leaving an explicit
        // transposition.
        res_ = rs_.transposition(e.shared_from_this());
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        res_ = rs_.rmul(transpose(e.sub()),
                        rs_.weightset()->transpose(e.weight()));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        res_ = rs_.lmul(rs_.weightset()->transpose(e.weight()),
                        transpose(e.sub()));
      }

      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }


    private:
      expressionset_t rs_;
      expression_t res_;
    };
  }

  /// A expression that denotes the transposition of \a v.
  ///
  /// Works deeply, contrary to transposition() that merely applies
  /// the `{T}` operator.
  template <typename ExpSet>
  typename ExpSet::value_t
  transpose(const ExpSet& rs, const typename ExpSet::value_t& v)
  {
    return rs.transpose(v);
  }

}
