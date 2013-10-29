#ifndef VCSN_DYN_POLYNOMIALSET_HH
# define VCSN_DYN_POLYNOMIALSET_HH

# include <memory>
# include <string>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      class polynomialset_base
      {
      public:
        /// A description of the polynomialset, sufficient to build it.
        /// \param full  whether to include the gensets.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        /// Convert a dynamic name into a static one.
        /// (from vname to sname, i.e., strip generators).
        static std::string sname(const std::string& vname);
      };
    }
    using polynomialset = std::shared_ptr<const detail::polynomialset_base>;
  }

}

#endif // !VCSN_DYN_POLYNOMIALSET_HH
