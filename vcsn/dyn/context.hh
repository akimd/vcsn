#ifndef VCSN_DYN_CONTEXT_HH
# define VCSN_DYN_CONTEXT_HH

# include <memory>
# include <string>

# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Template-less root for contexts.
      class abstract_context
      {
      public:
        /// A description of the context, sufficient to build it.
        /// \param full  whether to include the gensets.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        /// Convert a dynamic name into a static one.
        /// (from vname to sname, i.e., strip generators).
        static std::string sname(const std::string& vname);
      };
    }

    using context = std::shared_ptr<const detail::abstract_context>;
  }

}

#endif // !VCSN_DYN_CONTEXT_HH
