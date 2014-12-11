#ifndef VCSN_DYN_CONTEXT_HH
# define VCSN_DYN_CONTEXT_HH

# include <memory>
# include <string>

# include <vcsn/ctx/fwd.hh> // vcsn::context
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/export.hh>
# include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Template-less root for contexts.
      class LIBVCSN_API context_base
      {
      public:
        /// A description of the context, sufficient to build it.
        virtual symbol vname() const = 0;

        /// Downcast to the exact type.
        template <typename Ctx>
        auto& as()
        {
          return dynamic_cast<context_wrapper<Ctx>&>(*this).context();
        }

        /// Downcast to the exact type.
        template <typename Ctx>
        auto& as() const
        {
          return dynamic_cast<const context_wrapper<Ctx>&>(*this).context();
        }
      };

      /// A wrapped typed context.
      template <typename Context>
      class context_wrapper: public context_base
      {
      public:
        using context_t = Context;
        using super_t = context_base;

        context_wrapper(const context_t& context)
          : context_(context)
        {}

        virtual symbol vname() const override
        {
          return context().sname();
        }

        const context_t& context() const
        {
          return context_;
        }

      private:
        /// The context.
        const context_t context_;
      };

    } // namespace detail

    using context = std::shared_ptr<const detail::context_base>;

    template <typename LabelSet, typename WeightSet>
    inline
    context
    make_context(const vcsn::context<LabelSet, WeightSet>& ctx)
    {
      using context_t = vcsn::context<LabelSet, WeightSet>;
      return std::make_shared<detail::context_wrapper<context_t>>(ctx);
    }

  }

}

#endif // !VCSN_DYN_CONTEXT_HH
