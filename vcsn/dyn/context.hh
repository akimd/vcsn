#ifndef VCSN_DYN_CONTEXT_HH
# define VCSN_DYN_CONTEXT_HH

# include <memory>
# include <string>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/export.hh>

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
        /// \param full  whether to include the gensets.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        /// Convert a dynamic name into a static one.
        /// (from vname to sname, i.e., strip generators).
        static std::string sname(const std::string& vname);

        template <typename Ctx>
        Ctx as()
        {
          return dynamic_cast<context_wrapper<Ctx>&>(*this).context();
        }

        template <typename Ctx>
        const Ctx as() const
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

        virtual std::string vname(bool full = true) const override
        {
          return context().vname(full);
        }

        const context_t context() const
        {
          return context_;
        }

      protected:
        /// The context.
        const context_t context_;
      };

    } // namespace detail

    using context = std::shared_ptr<const detail::context_base>;

    template <typename Context>
    inline
    context
    make_context(const Context& ctx)
    {
      return std::make_shared<detail::context_wrapper<Context>>(ctx);
    }

  }

}

#endif // !VCSN_DYN_CONTEXT_HH
