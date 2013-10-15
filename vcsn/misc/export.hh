#ifndef VCSN_MISC_EXPORT_HH
# define VCSN_MISC_EXPORT_HH

# include <vcsn/misc/attributes.hh>

# ifdef BUILD_LIBVCSN
#   define LIBVCSN_API ATTRIBUTE_DLLEXPORT
# else
#   define LIBVCSN_API ATTRIBUTE_DLLIMPORT
# endif

#endif // !VCSN_MISC_EXPORT_HH
