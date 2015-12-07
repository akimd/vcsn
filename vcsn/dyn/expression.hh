#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// An abstract expression.
      class LIBVCSN_API expression_base
      {
      public:
        /// A description of the expression type.
        virtual symbol vname() const = 0;

        /// Extract wrapped typed expression.
        template <typename ExpSet>
        auto& as()
        {
          return dyn_cast<expression_wrapper<ExpSet>&>(*this);
        }

        /// Extract wrapped typed expression.
        template <typename ExpSet>
        auto& as() const
        {
          return dyn_cast<const expression_wrapper<ExpSet>&>(*this);
        }
      };


      /// Aggregate an expression and its expressionset.
      template <typename ExpSet>
      class expression_wrapper final: public expression_base
      {
      public:
        using expressionset_t = ExpSet;
        using super_t = expression_base;
        using expression_t = typename expressionset_t::value_t;
        expression_wrapper(const expressionset_t& rs,
                           const expression_t& r)
          : expressionset_(rs)
          , expression_(r)
        {}

        virtual symbol vname() const override
        {
          return expressionset().sname();
        }

        const expressionset_t& expressionset() const
        {
          return expressionset_;
        }

        const expression_t expression() const
        {
          return expression_;
        }

      private:
        /// The expression set.
        const expressionset_t expressionset_;
        /// The expression.
        const expression_t expression_;
      };

      /// Return the join of the expressionset of two expressions and their
      /// values in this new expressionset.
      template <typename ExpSetLhs, typename ExpSetRhs>
      auto
      join(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return std::make_tuple(rs, lr, rr);
      }

    } // namespace detail

    using expression = std::shared_ptr<detail::expression_base>;

    template <typename ExpSet>
    inline
    expression
    make_expression(const ExpSet& rs,
                    const typename ExpSet::value_t& r)
    {
      using wrapper_t = detail::expression_wrapper<ExpSet>;
      return std::make_shared<wrapper_t>(rs, r);
    }

  } // namespace dyn
} // namespace vcsn
