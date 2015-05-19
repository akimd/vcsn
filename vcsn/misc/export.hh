#pragma once

#include <vcsn/misc/attributes.hh>

#ifdef BUILD_LIBVCSN
#  define LIBVCSN_API ATTRIBUTE_DLLEXPORT
#else
#  define LIBVCSN_API ATTRIBUTE_DLLIMPORT
#endif
