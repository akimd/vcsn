#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// An dyn polynomial.
      class polynomial
      {
      public:
        polynomial()
          : self_()
        {}

        template <typename PolynomialSet>
        polynomial(const PolynomialSet& ps, const typename PolynomialSet::value_t& p)
          : self_(std::make_shared<model<PolynomialSet>>(ps, p))
        {}

        /// A description of the value type.
        symbol vname() const
        {
          return self_->vname();
        }

        template <typename PolynomialSet>
        auto& as()
        {
          return dyn_cast<model<PolynomialSet>&>(*self_);
        }

        template <typename PolynomialSet>
        auto& as() const
        {
          return dyn_cast<const model<PolynomialSet>&>(*self_);
        }

        auto* operator->()
        {
          return this;
        }

        const auto* operator->() const
        {
          return this;
        }

        bool empty() const
        {
          return self_->empty();
        }

      private:
        /// Abstract wrapped typed label/labelset.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
          virtual bool empty() const = 0;
        };

        /// Aggregate a value and its valueset.
        template <typename ValueSet>
        struct model final: base
        {
        public:
          using valueset_t = ValueSet;
          using value_t = typename valueset_t::value_t;

          model(const valueset_t& vs, const value_t& v)
            : valueset_(vs)
            , value_(v)
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

          /// Whether is zero.
          virtual bool empty() const override
          {
            return value_.empty();
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

      /// Return the join of the polynomialset of two polynomials and their
      /// values in this new polynomialset.
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      auto
      join(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.valueset(), r.valueset());
        auto lr = rs.conv(l.valueset(), l.value());
        auto rr = rs.conv(r.valueset(), r.value());
        return std::make_tuple(rs, lr, rr);
      }

    } // namespace detail

    using polynomial = detail::polynomial;
  } // namespace dyn
} // namespace vcsn
