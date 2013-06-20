#ifndef VCSN_WEIGHTSET_HH
# define VCSN_WEIGHTSET_HH

# include <memory>
# include <string>

# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      class abstract_weightset
      {
      public:
        /// A description of the weightset, sufficient to build it.
        /// \param full  whether to include the gensets.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        /// Convert a dynamic name into a static one.
        /// (from vname to sname, i.e., strip generators).
        static std::string sname(const std::string& vname);
      };
    }
    using weightset = std::shared_ptr<const detail::abstract_weightset>;
  }

}

#endif // !VCSN_WEIGHTSET_HH
