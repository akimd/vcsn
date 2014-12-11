#ifndef LIB_VCSN_RAT_READ_HH
# define LIB_VCSN_RAT_READ_HH

# include <vcsn/dyn/fwd.hh>
# include <lib/vcsn/rat/location.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace rat LIBVCSN_API
  {
    /// The expression in stream \a is, with \a rs as default expressionset.
    dyn::expression
    read(const dyn::expressionset& rs, std::istream& is,
         const location& l = location{});
  }
}

#endif // !LIB_VCSN_RAT_READ_HH
