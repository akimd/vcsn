# Check for concepts-lite support.
#
# Not used, as we can rely on #if defined __cpp_concepts.
AC_DEFUN([AX_CXX_CONCEPTS],
[AC_CACHE_CHECK([whether concepts lite are supported],
               [ac_cv_cxx_have_concepts_lite],
[AC_COMPILE_IFELSE([AC_LANG_SOURCE(
[[
  template <typename T>
  concept bool Transmogrifiable()
  {
    return requires (T t)
      {
        typename T::transmogrified_type;
        { t.transmogrify() } -> typename T::transmogrified_type;
      };
  }
]])],
    [ac_cv_cxx_have_concepts_lite=yes],
    [ac_cv_cxx_have_concepts_lite=no])])
case $ac_cv_cxx_have_concepts_lite in (yes)
  AC_DEFINE([HAVE_CONCEPTS_LITE], 1,
            [Define to 1 if concepts lite are supported])
esac
])
