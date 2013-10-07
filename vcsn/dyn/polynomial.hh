#ifndef VCSN_DYN_POLYNOMIAL_HH
# define VCSN_DYN_POLYNOMIAL_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/polynomialset.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// Aggregate a polynomial and its polynomialset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static polynomials.
      class abstract_polynomial
      {
      public:
        /// A description of the polynomial.
        /// \param full  whether to include the genset.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        template <typename PolynomialSet>
        concrete_abstract_polynomial<PolynomialSet>& as()
        {
          return dynamic_cast<concrete_abstract_polynomial<PolynomialSet>&>(*this);
        };

        template <typename PolynomialSet>
        const concrete_abstract_polynomial<PolynomialSet>& as() const
        {
          return dynamic_cast<const concrete_abstract_polynomial<PolynomialSet>&>(*this);
        };

        /// Whether is zero.
        virtual bool empty() const = 0;
      };

      /// Aggregate a polynomial and its polynomialset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static polynomials.
      template <typename PolynomialSet>
      class concrete_abstract_polynomial: public abstract_polynomial
      {
      public:
        using polynomialset_t = PolynomialSet;
        using super_type = abstract_polynomial;
        using polynomial_t = typename polynomialset_t::value_t;
        concrete_abstract_polynomial(const polynomial_t& polynomial,
                                     const polynomialset_t& polynomialset)
          : polynomial_(polynomial)
          , polynomialset_(polynomialset)
        {}
        virtual ~concrete_abstract_polynomial() {}

        virtual std::string vname(bool full = true) const override
        {
          return get_polynomialset().vname(full);
        }

        const polynomial_t polynomial() const
        {
          return polynomial_;
        }

        const polynomialset_t& get_polynomialset() const
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

    using polynomial = std::shared_ptr<const detail::abstract_polynomial>;

    template <typename PolynomialSet>
    inline
    polynomial
    make_polynomial(const PolynomialSet& ps,
                    const typename PolynomialSet::value_t& polynomial)
    {
      return std::make_shared<detail::concrete_abstract_polynomial<PolynomialSet>>
        (polynomial, ps);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_POLYNOMIAL_HH
