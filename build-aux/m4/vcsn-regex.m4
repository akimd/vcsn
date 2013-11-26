#                                                       -*- Autoconf -*-

m4_pattern_forbid([^VCSN])

# VCSN_REGEX
# ----------
# Check if <regex> works.  If it does not, use Boost.Regex.
# Unforunately, GCC cannot be tested just at compile-time.
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53631.
AC_DEFUN([VCSN_REGEX],
[AC_CACHE_CHECK([whether std::regex works], [vcsn_cv_std_regex_works],
  [AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([[
      #include <iostream>
      #include <regex>
      ]],
      [[
        try
          {
            std::string target = "(*2)",
                        re = "\\\\(\\\\*([0-9]*)(,?)([0-9]*)\\\\)";
            std::regex regex{re, std::regex::extended};
            std::smatch match;
            if (!std::regex_match(target, match, regex))
              throw std::runtime_error(target + " !~ " + re);
            for (const auto& s: match)
              std::cerr << s << std::endl;
          }
        catch (std::exception& e)
          {
            std::cerr << e.what() << std::endl;
            return 1;
          }
      ]])],
    [vcsn_cv_std_regex_works=yes],
    [vcsn_cv_std_regex_works="no (fails at runtime)"],
    [vcsn_cv_std_regex_works="do not know (cross-compilation)"])])

case $vcsn_cv_std_regex_works in
  (yes)
    AC_DEFINE([STD_REGEX_WORKS], 1, [Define if <regex> works.])
    ;;
  (*)
    BOOST_REGEX
    ;;
esac
])
