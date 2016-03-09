#pragma once

#include <memory>
#include <string>

#include <vcsn/ctx/fwd.hh> // vcsn::context
#include <vcsn/dyn/cast.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Template-less root for contexts.
      class LIBVCSN_API context
      {
      public:
        context()
          : self_(nullptr)
        {}

        /// A description of the context, sufficient to build it.
        template <typename LabelSet, typename WeightSet>
        context(const vcsn::context<LabelSet, WeightSet>& ctx)
          : self_(std::make_shared<model<vcsn::context<LabelSet, WeightSet>>>(ctx))
        {}

        symbol vname() const
        {
          return self_->vname();
        }

        /// Downcast to the exact type.
        template <typename Ctx>
        auto& as()
        {
          return dyn_cast<model<Ctx>&>(*self_).context();
        }

        /// Downcast to the exact type.
        template <typename Ctx>
        auto& as() const
        {
          return dyn_cast<const model<Ctx>&>(*self_).context();
        }

        const auto* operator->() const
        {
          return this;
        }

        operator bool() const
        {
          return bool(self_);
        }

      private:
        /// Abstract wrapped type.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// A wrapped type.
        template <typename Context>
        struct model final : base
        {
          using context_t = Context;

          model(context_t ctx)
            : context_(std::move(ctx))
          {}

          symbol vname() const
          {
            return context().sname();
          }

          const auto& context() const
          {
            return context_;
          }

          /// The context.
          const context_t context_;
        };

        /// The wrapped context.
        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using context = detail::context;

    /// Build a dyn::context from a static one.
    template <typename LabelSet, typename WeightSet>
    inline
    context
    make_context(const vcsn::context<LabelSet, WeightSet>& ctx)
    {
      return context(ctx);
    }
  }
}
