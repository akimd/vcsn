#ifndef LIB_VCSN_RAT_READ_HH
# define LIB_VCSN_RAT_READ_HH

# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace rat LIBVCSN_API
  {
    /// The ratexp in file \a f, with \a rs as default ratexpset.
    dyn::ratexp read_file(const std::string& f, const dyn::ratexpset& rs);

    /// The ratexp in string \a s, with \a rs as default ratexpset.
    dyn::ratexp read_string(const std::string& s, const dyn::ratexpset& rs);
  }
}

#endif // ! LIB_VCSN_RAT_READ_HH
