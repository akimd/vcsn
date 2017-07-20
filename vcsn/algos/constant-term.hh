#pragma once

#include <vcsn/algos/project.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /// The constant term of \a e.
  template <typename ExpSet>
  weight_t_of<ExpSet>
  constant_term(const ExpSet& rs, const typename ExpSet::value_t& e);

  namespace rat
  {

    /*-----------------------------.
    | constant_term(expression).   |
    `-----------------------------*/

    /// A functor to compute the constant term of an expression.
    ///
    /// \tparam ExpSet  the expressionset type.
    template <typename ExpSet>
    class constant_term_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = constant_term_visitor;

      using context_t = context_t_of<expressionset_t>;
      using expression_t = typename expressionset_t::value_t;
      using weight_t = weight_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<expressionset_t>;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "constant_term"; }

      constant_term_visitor(const expressionset_t& rs)
        : rs_{rs}
        , ws_{*rs_.weightset()}
      {}

      weight_t operator()(const expression_t& v)
      {
        try
          {
            return constant_term(v);
          }
        catch (const std::runtime_error& e)
          {
            raise(e, "  while computing constant-term of: ", to_string(rs_, v));
          }

      }

    private:
      /// Easy recursion.
      weight_t constant_term(const expression_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ws_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = ws_.one();
      }

      VCSN_RAT_VISIT(atom,)
      {
        res_ = ws_.zero();
      }

      VCSN_RAT_VISIT(name, v)
      {
        res_ = constant_term(v.sub());
      }

      VCSN_RAT_VISIT(add, v)
      {
        weight_t res = ws_.zero();
        for (auto c: v)
          res = ws_.add(res, constant_term(c));
        res_ = std::move(res);
      }

      /// Visit a variadic node whose constant-term is the product of
      /// the constant-terms of its children.
      template <typename Node>
      void visit_product(const Node& v)
      {
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(conjunction, v) { visit_product(v); }
      VCSN_RAT_VISIT(infiltrate, v)  { visit_product(v); }
      VCSN_RAT_VISIT(mul, v)         { visit_product(v); }
      VCSN_RAT_VISIT(shuffle, v)     { visit_product(v); }

      /// Cannot compute the constant-term easily here:
      /// `c(<x>a{\}<y>a) = x{\}y`, yet both operands have a null
      /// constant-term.
      VCSN_RAT_UNSUPPORTED(ldivide);

      /// Cannot compute the constant-term easily here:
      /// `c(<x>\\e|x @ <y>x|\\e) = xy`, yet both operands have a null
      /// constant-term.
      VCSN_RAT_UNSUPPORTED(compose);

      VCSN_RAT_VISIT(transposition, v)
      {
        res_ = ws_.transpose(constant_term(v.sub()));
      }

      VCSN_RAT_VISIT(star, v)
      {
        res_ = ws_.star(constant_term(v.sub()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = ws_.mul(v.weight(), constant_term(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = ws_.mul(constant_term(v.sub()), v.weight());
      }

      VCSN_RAT_VISIT(complement, v)
      {
        res_
          = ws_.is_zero(constant_term(v.sub()))
          ? ws_.one()
          : ws_.zero();
      }

      /*---------.
      | tuple.   |
      `---------*/

      using tuple_t = typename super_t::tuple_t;
      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Constant term for one tape.
        template <size_t I>
        weight_t work_(const tuple_t& v)
        {
          return ::vcsn::constant_term(visitor_.rs_.template project<I>(),
                                       std::get<I>(v.sub()));
        }

        /// Product of the constant-terms of all tapes.
        template <size_t... I>
        weight_t work_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return visitor_.ws_.mul(work_<I>(v)...);
        }

        /// Entry point.
        weight_t operator()(const tuple_t& v)
        {
          return work_(v, labelset_t_of<context_t>::indices);
        }
        const self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        weight_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        const self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        res_ = visit_tuple<>{*this}(v);
      }

    private:
      expressionset_t rs_;
      weightset_t ws_;
      weight_t res_;
    };
  } // rat::

  template <typename ExpSet>
  weight_t_of<ExpSet>
  constant_term(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    auto c = rat::constant_term_visitor<ExpSet>{rs};
    return c(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      weight
      constant_term(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return {*e.valueset().weightset(),
                constant_term<ExpSet>(e.valueset(), e.value())};
      }
    }
  }
} // vcsn::
