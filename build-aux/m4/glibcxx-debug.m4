
# VCSN_GLIBCXX_DEBUG
# ------------------
# If needed, check for REGEX before _GLIBCXX_DEBUG (with VCSN_REGEX).
AC_DEFUN([VCSN_GLIBCXX_DEBUG],
[AC_CACHE_CHECK([whether to enable _GLIBCXX_DEBUG],
               [ac_cv_GLIBCXX_DEBUG],
[# Set to "yes" and let each test so to "no" on failure.
ac_cv_GLIBCXX_DEBUG=yes

# Makes sense only for libstdc++.
if test "$ac_cv_GLIBCXX_DEBUG" = yes; then
  AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE(
  [[#ifndef __GLIBCXX__
   error: not libstdc++
   #endif
  ]])],
    [],
    [ac_cv_GLIBCXX_DEBUG='no (not using libstdc++)'])
fi


# I, Akim, on Erebus (Mac OS X, G++ 4.8) have errors like:
#
#   $ vcsn-cat -f share/vcsn/lal_char_z/c1.gv
#   vcsn-cat(78451) malloc: *** error for object 0x10ae577c0:
#                           pointer being freed was not allocated
#   *** set a breakpoint in malloc_error_break to debug
#
# when I use NDEBUG with _GLIBCXX_DEBUG.  AFAICT, this is not our
# fault, and I am tired to trying to fight such issues.  Since after
# all, it does not make a lot of sense to use _GLIBCXX_DEBUG with
# NDEBUG, let's use the easy way.
if test "$ac_cv_GLIBCXX_DEBUG" = yes; then
  AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE(
  [[#ifdef NDEBUG
   error: NDEBUG is enabled
   #endif
  ]])],
    [],
    [ac_cv_GLIBCXX_DEBUG='no (incompatible with NDEBUG)'])
fi


# For some reason, on OS X, enabling _GLIBCXX_DEBUG breaks the
# stringstreams.  This is well known, ancient, yet still the case
# today (G++ 4.7, XCode 4.2).
#
# http://stackoverflow.com/questions/7623070
if test "$ac_cv_GLIBCXX_DEBUG" = yes; then
  AC_RUN_IFELSE(
  [AC_LANG_PROGRAM([#define _GLIBCXX_DEBUG
  #define _GLIBCXX_DEBUG_PEDANTIC
  #include <cassert>
  #include <sstream>
  ],[
    {
      std::stringstream o;
      assert (o.str() == "");
    }
    {
      std::stringstream o;
      o << 42;
      assert (o.str() == "42");
    }
  ])], [],
       [ac_cv_GLIBCXX_DEBUG='no (breaks stringstreams)'],
       [ac_cv_GLIBCXX_DEBUG='no (cross-compilation)'])
fi


# _GLIBCXX_DEBUG may cause std::regex to lead to multiple symbol
# definitions.
#
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61329
if test "$ac_cv_GLIBCXX_DEBUG" = yes; then
  AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
  #define _GLIBCXX_DEBUG
  #define _GLIBCXX_DEBUG_PEDANTIC
  #include <regex>
  std::regex r1;]])],
                    [mv conftest.$OBJEXT conftest1.$OBJEXT])
  save_LIBS=$LIBS
  LIBS="$LIBS conftest1.$OBJEXT"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  #define _GLIBCXX_DEBUG
  #define _GLIBCXX_DEBUG_PEDANTIC
  #include <regex>
  ]],
  [[std::regex r2;]])],
                 [],
                 [ac_cv_GLIBCXX_DEBUG='no (multiple symbols at link time)'])
  rm -f conftest1.$OBJEXT
  LIBS=$save_LIBS
fi


# If we use Boost.Regex, do not enable _GLIBCXX_DEBUG if the result fails.
#
# https://svn.boost.org/trac/boost/ticket/5911
if test "$ac_cv_GLIBCXX_DEBUG" = yes && test -n "$BOOST_REGEX_LIBS"; then
  vcsn_save_LIBS=$LIBS
  LIBS="$LIBS $BOOST_REGEX_LDFLAGS $BOOST_REGEX_LIBS"
  AC_RUN_IFELSE(
  [AC_LANG_PROGRAM([#define _GLIBCXX_DEBUG
  #define _GLIBCXX_DEBUG_PEDANTIC
  #include <cassert>
  #include <sstream>
  #include <boost/regex.hpp>
  ],[
    {
      const boost::regex re("[a-zA-Z]*");
      assert(boost::regex_match("asdf", re));
    }
  ])], [],
       [ac_cv_GLIBCXX_DEBUG='no (breaks Boost.Regexp)'])
  LIBS=$vcsn_save_LIBS
fi
])
])
