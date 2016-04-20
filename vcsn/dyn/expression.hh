#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/cast.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// An dyn expression.
      class expression
      {
      public:
        expression()
          : self_()
        {}

        // It's must have a nicest way
        expression(std::nullptr_t null)
          : self_(null)
        {}

        template <typename ExpSet>
        expression(const ExpSet& es, const typename ExpSet::value_t& e)
          : self_(std::make_shared<model<ExpSet>>(es, e))
        {}

        /// A description of the value type.
        symbol vname() const
        {
          return self_->vname();
        }
        /// Extract wrapped typed expression.
        template <typename ExpSet>
        auto& as()
        {
          return dyn_cast<model<ExpSet>&>(*self_);
        }

        /// Extract wrapped typed expression.
        template <typename ExpSet>
        auto& as() const
        {
          return dyn_cast<const model<ExpSet>&>(*self_);
        }

        auto* operator->()
        {
          return this;
        }

        const auto* operator->() const
        {
          return this;
        }

        auto& operator=(const expression& e)
        {
          self_ = std::move(e.self_);
          return *this;
        }

      private:
        /// Abstract wrapped typed expression/expressionset.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// Aggregate an expression and its expressionset.
        template <typename ValueSet>
        struct model final: base
        {
          using valueset_t = ValueSet;
          using value_t = typename valueset_t::value_t;

          model(const valueset_t& rs, const value_t& r)
            : valueset_(rs)
            , value_(r)
          {}

          virtual symbol vname() const override
          {
            return valueset().sname();
          }

          const valueset_t& valueset() const
          {
            return valueset_;
          }

          const value_t value() const
          {
            return value_;
          }

        private:
          /// The value set.
          const valueset_t valueset_;
          /// The value.
          const value_t value_;
        };

        /// The wrapped expression/expressionset.
        std::shared_ptr<base> self_;
      };

      /// Return the join of the expressionset of two expressions and their
      /// values in this new expressionset.
      template <typename ExpSetLhs, typename ExpSetRhs>
      auto
      join(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.valueset(), r.valueset());
        auto lr = rs.conv(l.valueset(), l.value());
        auto rr = rs.conv(r.valueset(), r.value());
        return std::make_tuple(rs, lr, rr);
      }

    } // namespace detail

    using expression = detail::expression;

  } // namespace dyn
} // namespace vcsn
