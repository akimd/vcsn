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

      /// An abstract polynomial.
      class polynomial_base
      {
      public:
        /// A description of the polynomial.
        virtual symbol vname() const = 0;

        template <typename PolynomialSet>
        auto& as()
        {
          return dyn_cast<polynomial_wrapper<PolynomialSet>&>(*this);
        }

        template <typename PolynomialSet>
        auto& as() const
        {
          return dyn_cast<const polynomial_wrapper<PolynomialSet>&>(*this);
        }

        /// Whether is zero.
        virtual bool empty() const = 0;
      };

      /// Aggregate a polynomial and its polynomialset.
      template <typename PolynomialSet>
      class polynomial_wrapper final: public polynomial_base
      {
      public:
        using polynomialset_t = PolynomialSet;
        using super_t = polynomial_base;
        using polynomial_t = typename polynomialset_t::value_t;
        polynomial_wrapper(const polynomialset_t& ps, const polynomial_t& p)
          : polynomialset_(ps)
          , polynomial_(p)
        {}
        virtual ~polynomial_wrapper() {}

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

    using polynomial = std::shared_ptr<const detail::polynomial_base>;

    template <typename PolynomialSet>
    inline
    polynomial
    make_polynomial(const PolynomialSet& ps,
                    const typename PolynomialSet::value_t& p)
    {
      using wrapper_t = detail::polynomial_wrapper<PolynomialSet>;
      return std::make_shared<wrapper_t>(ps, p);
    }
  } // namespace dyn
} // namespace vcsn
