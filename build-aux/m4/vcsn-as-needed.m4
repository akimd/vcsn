# https://wiki.ubuntu.com/ToolChain/CompilerFlags#A-Wl.2C--as-needed
#
# On some distros shared libraries that appear to be useless (i.e.,
# they don't provide symbols that were actually used by the object
# that was linked against them) are not kept as dependencies,
# therefore they are not loaded, and their initialization section is
# not executed either.
#
# This is bad for Vcsn which uses the libraries' initialization
# to register their content to the main executable.  In other words,
# because we perform the "linking" ourselves (via our registries), the
# linker does not see that we need these libraries.
#
# So find if this compiler set up actually discards the "useless"
# libraries, and then provide a means to force the loading of such
# libraries.

AC_DEFUN([_VCSN_LDFLAGS_AS_NEEDED],
[CXX_save=$CXX
CXX="$CXX -fPIC"
LDFLAGS_save=$LDFLAGS
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <cstdlib>
bool reg () { exit (0); return true; }
static bool registered = reg ();
]])], [mv conftest.$OBJEXT reg.$OBJEXT],
      [AC_MSG_ERROR([cannot compile simple C++ code])])
if $CXX -shared -o libreg.so $CXXFLAGS $CPPFLAGS $LDFLAGS reg.$OBJEXT $LIBS >&AS_MESSAGE_LOG_FD; then
  # Use -rpath to avoid the need for setting LD_LIBRARY_PATH.  This is
  # very compiler dependent, but currently, for Vcsn, we don't
  # care much about this: GCC and Clang accept this perfectly.
  #
  # Changing LD_LIBRARY_PATH would require rewriting AC_RUN_IFELSE,
  # which does not sound like a good idea.
  LDFLAGS="$LDFLAGS -L. -lreg -Wl,-rpath,."
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
int main ()
{
  // If the shared library is properly loaded, and initialized,
  // then its call to exit(0) will be executed before this return.
  return 1;
}
]])],
  [$1=no],
  [$1=yes],
  [$1="don't know (cross-compiling)"])
  rm -f libreg.so reg.$OBJEXT
fi
CXX=$CXX_save
LDFLAGS=$LDFLAGS_save
])

# Unfortunately when one passes -Wl,--no-as-needed to libtool, it
# moves the flag around, and actually _last_, even _after_ that the
# libraries were listed.  So, although -Wl,--no-as-needed is passed,
# since it's last, it's useless.
#
# The workaround, taken from the Libtool FAQ, is to pass these flags
# directly in the compiler name.
AC_DEFUN([VCSN_LDFLAGS_AS_NEEDED],
[AC_CACHE_CHECK([whether -Wl,--no-as-needed is needed],
                [vcsn_cv_no_as_needed_needed],
                [_VCSN_LDFLAGS_AS_NEEDED([vcsn_cv_no_as_needed_needed])])
case $vcsn_cv_no_as_needed_needed in
  (yes) CXX="$CXX -Wl,--no-as-needed" ;;
esac
])
