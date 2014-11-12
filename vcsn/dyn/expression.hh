#ifndef VCSN_DYN_EXPRESSION_HH
# define VCSN_DYN_EXPRESSION_HH

# include <memory>
# include <string>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/export.hh>
# include <vcsn/misc/symbol.hh>

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
        template <typename RatExpSet>
        expression_wrapper<RatExpSet>& as()
        {
          return dynamic_cast<expression_wrapper<RatExpSet>&>(*this);
        }

        /// Extract wrapped typed expression.
        template <typename RatExpSet>
        const expression_wrapper<RatExpSet>& as() const
        {
          return dynamic_cast<const expression_wrapper<RatExpSet>&>(*this);
        }
      };


      /// Aggregate a expression and its expressionset.
      template <typename RatExpSet>
      class expression_wrapper: public expression_base
      {
      public:
        using expressionset_t = RatExpSet;
        using super_t = expression_base;
        using expression_t = typename expressionset_t::value_t;
        expression_wrapper(const expression_t& expression,
                       const expressionset_t& expressionset)
          : expression_(expression)
          , expressionset_(expressionset)
        {}

        virtual symbol vname() const override
        {
          return expressionset().sname();
        }

        const expression_t expression() const
        {
          return expression_;
        }

        const expressionset_t& expressionset() const
        {
          return expressionset_;
        }

      protected:
        /// The expression.
        const expression_t expression_;
        /// The expression set.
        const expressionset_t expressionset_;
      };

    } // namespace detail

    using expression = std::shared_ptr<detail::expression_base>;

    template <typename RatExpSet>
    inline
    expression
    make_expression(const RatExpSet& rs,
                const typename RatExpSet::value_t& expression)
    {
      using wrapper_t = detail::expression_wrapper<RatExpSet>;
      return std::make_shared<wrapper_t>(expression, rs);
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_EXPRESSION_HH
