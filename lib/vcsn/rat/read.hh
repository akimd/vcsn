#ifndef LIB_VCSN_RAT_READ_HH
# define LIB_VCSN_RAT_READ_HH

# include <vcsn/dyn/fwd.hh>
# include <lib/vcsn/rat/location.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace rat LIBVCSN_API
  {
    /// The ratexp in stream \a is, with \a rs as default ratexpset.
    dyn::ratexp read(std::istream& is, const dyn::ratexpset& rs,
                     const location& l = location{});
  }
}

#endif // ! LIB_VCSN_RAT_READ_HH
