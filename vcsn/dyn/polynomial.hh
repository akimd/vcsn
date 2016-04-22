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

        /// Aggregate a polynomial and its polynomialset.
        template <typename PolynomialSet>
        struct model final: base
        {
        public:
          using polynomialset_t = PolynomialSet;
          using polynomial_t = typename polynomialset_t::value_t;

          model(const polynomialset_t& ps, const polynomial_t& p)
            : polynomialset_(ps)
            , polynomial_(p)
          {}

          virtual symbol vname() const override
          {
            return polynomialset().sname();
          }

          const polynomialset_t& polynomialset() const
          {
            return polynomialset_;
          }

          const polynomial_t polynomial() const
          {
            return polynomial_;
          }

          /// Whether is zero.
          virtual bool empty() const override
          {
            return polynomial_.empty();
          }

        private:
          /// The polynomial set.
          const polynomialset_t polynomialset_;
          /// The polynomial.
          const polynomial_t polynomial_;
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
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return std::make_tuple(rs, lr, rr);
      }

    } // namespace detail

    using polynomial = detail::polynomial;
  } // namespace dyn
} // namespace vcsn
