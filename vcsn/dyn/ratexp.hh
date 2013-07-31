#ifndef VCSN_DYN_RATEXP_HH
# define VCSN_DYN_RATEXP_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/context.hh> // vname.
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// Aggregate an expression and its context.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static ratexps.
      class abstract_ratexp
      {
      public:
        using exp_t = vcsn::rat::exp_t;
        abstract_ratexp(const dyn::context& ctx, const exp_t& ratexp)
          : ctx_(ctx)
          , ratexp_(ratexp)
        {}
        ~abstract_ratexp() {}

        const dyn::detail::abstract_context& ctx() const
        {
          return *ctx_;
        }
        const exp_t& ratexp() const
        {
          return ratexp_;
        }

        template <typename RatExpSet>
        const exp_t&as() const
        {
          return ratexp_;
        }

        std::string vname(bool full = true) const
        {
          // FIXME: this is a fake, to be cleaned once we really
          // have moved the implementation of dyn::ratexps.
          return std::string("ratexpset<") + ctx().vname(full) + ">";
        }

      protected:
        /// A shared_ptr to the abstract_context.
        const dyn::context ctx_;
        /// A shared_ptr to the abstract_ratexp.
        const exp_t ratexp_;
      };

    } // namespace detail

    using ratexp = std::shared_ptr<detail::abstract_ratexp>;

    inline
    ratexp
    make_ratexp(const dyn::context& ctx, const rat::exp_t& ratexp)
    {
      return std::make_shared<ratexp::element_type>(ctx, ratexp);
    }

    template <typename LabelSet, typename WeightSet>
    inline
    ratexp
    make_ratexp(const ctx::context<LabelSet, WeightSet>& ctx,
                const rat::exp_t& ratexp)
    {
      using ctx_t = ctx::context<LabelSet, WeightSet>;
      return make_ratexp(std::make_shared<const ctx_t>(ctx), ratexp);
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_RATEXP_HH
