#ifndef VCSN_DYN_WEIGHT_HH
# define VCSN_DYN_WEIGHT_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace details
    {

      /// Aggregate a weight and its context.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static weights.
      class abstract_weight
      {
      public:
        abstract_weight(const dyn::context& ctx)
          : ctx_(ctx)
        {}
        virtual ~abstract_weight() {}

        const dyn::abstract_context& ctx() const
        {
          return *ctx_;
        }
      protected:
        /// A shared_ptr to the abstract_context.
        const dyn::context ctx_;
      };

      /// Aggregate a weight and its context.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static weights.
      template <typename Ctx>
      class concrete_abstract_weight: public abstract_weight
      {
      public:
        using super_type = abstract_weight;
        using context_t = Ctx;
        using weight_t = typename context_t::weight_t;
        concrete_abstract_weight(const dyn::context& ctx,
                                 const weight_t& weight)
          : super_type(ctx)
          , weight_(weight)
        {}
        virtual ~concrete_abstract_weight() {}

        const context_t& ctx() const
        {
          return dynamic_cast<const context_t&>(*ctx_);
        }
        const weight_t& weight() const
        {
          return weight_;
        }
      protected:
        /// The weight.
        const weight_t weight_;
      };

    } // namespace details

    using weight = std::shared_ptr<const details::abstract_weight>;

    template <typename LabelSet, typename WeightSet>
    inline
    weight
    make_weight(const ctx::context<LabelSet, WeightSet>& ctx,
                const typename WeightSet::value_t& weight)
    {
      using ctx_t = ctx::context<LabelSet, WeightSet>;
      return std::make_shared<details::concrete_abstract_weight<ctx_t>>
        (std::make_shared<const ctx_t>(ctx), weight);
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_WEIGHT_HH
