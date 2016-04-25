#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/cast.hh>
#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Tag for expansion/expansionset
      struct expansion_tag
      {};

      /// Tag for expression/expressionset
      struct expression_tag
      {};

      /// Tag for label/labelset.
      struct label_tag
      {};

      /// Tag for polynomial/polynomialset
      struct polynomial_tag
      {};

      /// Tag for weight/weightset.
      struct weight_tag
      {};

      /// A dyn value/valueset.
      template <typename Tag>
      class LIBVCSN_API value
      {
      public:
        value(std::nullptr_t null = nullptr)
          : self_(null)
        {}

        template <typename ValueSet>
        value(const ValueSet& ls, const typename ValueSet::value_t& l)
          : self_(std::make_shared<model<ValueSet>>(ls, l))
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
          return dyn_cast<model<ValueSet>&>(*self_);
        }

        /// Extract wrapped typed value.
        template <typename ValueSet>
        const auto& as() const
        {
          return dyn_cast<const model<ValueSet>&>(*self_);
        }

        value* operator->()
        {
          return this;
        }

        const value* operator->() const
        {
          return this;
        }

        bool operator!()
        {
          return !self_;
        }

        auto& operator=(const value& l)
        {
          self_ = std::move(l.self_);
          return *this;
        }

      private:
        /// Abstract wrapped typed label/labelset.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// A wrapped typed label/labelset.
        template <typename ValueSet>
        struct model final : base
        {
          using valueset_t = ValueSet;
          using value_t = typename valueset_t::value_t;

          model(const valueset_t& ls, const value_t& l)
            : valueset_(std::move(ls))
            , value_(std::move(l))
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

        /// The wrapped value/valueset.
        std::shared_ptr<base> self_;
      };

      /// Return the join of the expressionset (or polynomialset) of two
      /// expressions (polynomials) and their values in this new
      /// expressionset (polynomialset).
      template <typename ValueSetLhs, typename ValueSetRhs, typename Tag>
      auto
      join(const value<Tag>& lhs, const value<Tag>& rhs)
      {
        const auto& l = lhs->template as<ValueSetLhs>();
        const auto& r = rhs->template as<ValueSetRhs>();
        auto rs = join(l.valueset(), r.valueset());
        auto lr = rs.conv(l.valueset(), l.value());
        auto rr = rs.conv(r.valueset(), r.value());
        return std::make_tuple(rs, lr, rr);
      }

    } // namespace detail

    // A class representing an expansion/expansionset.
    using expansion = detail::value<detail::expansion_tag>;
    // A class representing an expression/expressionset.
    using expression = detail::value<detail::expression_tag>;
    // A class representing a label/labelset.
    using label = detail::value<detail::label_tag>;
    // A class representing an polynomial/polynomialset.
    using polynomial = detail::value<detail::polynomial_tag>;
    // A class representing a weight/weightset.
    using weight = detail::value<detail::weight_tag>;

  } // namespace dyn
} // namespace vcsn
