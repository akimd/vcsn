#ifndef VCSN_DYN_POLYNOMIAL_HH
# define VCSN_DYN_POLYNOMIAL_HH

# include <memory>
# include <string>

# include <vcsn/dyn/fwd.hh>

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
        virtual std::string vname() const = 0;

        template <typename PolynomialSet>
        polynomial_wrapper<PolynomialSet>& as()
        {
          return dynamic_cast<polynomial_wrapper<PolynomialSet>&>(*this);
        }

        template <typename PolynomialSet>
        const polynomial_wrapper<PolynomialSet>& as() const
        {
          return dynamic_cast<const polynomial_wrapper<PolynomialSet>&>(*this);
        }

        /// Whether is zero.
        virtual bool empty() const = 0;
      };

      /// Aggregate a polynomial and its polynomialset.
      template <typename PolynomialSet>
      class polynomial_wrapper: public polynomial_base
      {
      public:
        using polynomialset_t = PolynomialSet;
        using super_t = polynomial_base;
        using polynomial_t = typename polynomialset_t::value_t;
        polynomial_wrapper(const polynomial_t& polynomial,
                           const polynomialset_t& polynomialset)
          : polynomial_(polynomial)
          , polynomialset_(polynomialset)
        {}
        virtual ~polynomial_wrapper() {}

        virtual std::string vname() const override
        {
          return polynomialset().sname();
        }

        const polynomial_t polynomial() const
        {
          return polynomial_;
        }

        const polynomialset_t& polynomialset() const
        {
          return polynomialset_;
        }

        /// Whether is zero.
        virtual bool empty() const override
        {
          return polynomial_.empty();
        }

      protected:
        /// The polynomial.
        const polynomial_t polynomial_;
        /// The polynomial set.
        const polynomialset_t polynomialset_;
      };

    } // namespace detail

    using polynomial = std::shared_ptr<const detail::polynomial_base>;

    template <typename PolynomialSet>
    inline
    polynomial
    make_polynomial(const PolynomialSet& ps,
                    const typename PolynomialSet::value_t& polynomial)
    {
      using wrapper_t = detail::polynomial_wrapper<PolynomialSet>;
      return std::make_shared<wrapper_t>(polynomial, ps);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_POLYNOMIAL_HH
