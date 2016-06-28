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
      /// Tag for expansion/expansionset.
      struct expansion_tag {};

      /// Tag for expression/expressionset.
      struct expression_tag {};

      /// Tag for label/labelset.
      struct label_tag {};

      /// Tag for polynomial/polynomialset.
      struct polynomial_tag {};

      /// Tag for weight/weightset.
      struct weight_tag {};
    }

    /// A dyn Value/ValueSet.
    template <typename Tag>
    class LIBVCSN_API value_impl
    {
    public:
      /// Default construction: empty.
      value_impl() {}

      template <typename ValueSet>
      value_impl(const ValueSet& vs, const typename ValueSet::value_t& v)
        : self_{std::make_shared<model<ValueSet>>(vs, v)}
      {}

      /// A description of the value type.
      symbol vname() const
      {
        return self_->vname();
      }

      /// Extract wrapped typed value.
      template <typename ValueSet>
      auto& as()
      {
        return detail::dyn_cast<model<ValueSet>&>(*self_);
      }

      /// Extract wrapped typed value.
      template <typename ValueSet>
      const auto& as() const
      {
        return detail::dyn_cast<const model<ValueSet>&>(*self_);
      }

      value_impl* operator->()
      {
        return this;
      }

      const value_impl* operator->() const
      {
        return this;
      }

      bool operator!()
      {
        return !self_;
      }

      auto& operator=(std::nullptr_t)
      {
        self_ = nullptr;
        return *this;
      }

      auto& operator=(const value_impl& l)
      {
        self_ = std::move(l.self_);
        return *this;
      }

    private:
      /// Abstract wrapped typed Value/ValueSet.
      struct base
      {
        virtual ~base() = default;
        virtual symbol vname() const = 0;
      };

      /// A wrapped typed Value/ValueSet.
      template <typename ValueSet>
      struct model final : base
      {
        using valueset_t = ValueSet;
        using value_t = typename valueset_t::value_t;
        model(valueset_t vs, value_t v)
          : valueset_(std::move(vs))
          , value_(std::move(v))
        {}

        symbol vname() const override
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

      /// The wrapped value/valueset.
      std::shared_ptr<base> self_ = nullptr;
    };

    /// Return the join of the valuesets of two dyn values and their
    /// values in this new valueset.
    template <typename ValueSetLhs, typename ValueSetRhs, typename Tag>
    auto
    join(const value_impl<Tag>& lhs, const value_impl<Tag>& rhs)
    {
      const auto& l = lhs->template as<ValueSetLhs>();
      const auto& r = rhs->template as<ValueSetRhs>();
      auto rs = join(l.valueset(), r.valueset());
      auto lr = rs.conv(l.valueset(), l.value());
      auto rr = rs.conv(r.valueset(), r.value());
      return std::make_tuple(rs, lr, rr);
    }

    // A class representing an expansion/expansionset.
    using expansion = value_impl<detail::expansion_tag>;
    // A class representing an expression/expressionset.
    using expression = value_impl<detail::expression_tag>;
    // A class representing a label/labelset.
    using label = value_impl<detail::label_tag>;
    // A class representing an polynomial/polynomialset.
    using polynomial = value_impl<detail::polynomial_tag>;
    // A class representing a weight/weightset.
    using weight = value_impl<detail::weight_tag>;
  } // namespace dyn
} // namespace vcsn
