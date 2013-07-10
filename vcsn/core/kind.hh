#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <type_traits>

namespace vcsn
{

  /// Define the kinds, and auxialary tools.
  ///
  /// is_ABBREV<Kinded>: Whether Kinded has a specific kind_t
  ///
  /// if_ABBREV<Kinded, R = Kinded>:
  /// Provide "overloading" on parameter types, for instance to require
  /// letter_t when labels_are_letters, and word_t when
  /// labels_are_words.  See ratexpset::atom and ratexpset::atom_ for
  /// an example.
  ///
  /// It is very tempting to turns these guys into members of
  /// ctx::context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  /// one must write "(typename Cxx::template if_lal<letter_t> v)".

# define DEFINE(Abbrev, Name)                                           \
  struct labels_are_ ## Name                                            \
  {                                                                     \
    static std::string name()                                           \
    {                                                                   \
      return "labels_are_" #Name;                                       \
    }                                                                   \
    static std::string sname()                                          \
    {                                                                   \
      return #Abbrev;                                                   \
    }                                                                   \
  };                                                                    \
                                                                        \
  template <typename Kinded>                                            \
  struct is_ ## Abbrev                                                  \
    : std::is_same<typename Kinded::kind_t, labels_are_ ## Name>::type  \
  {};                                                                   \
                                                                        \
  template <typename Kinded, typename R = Kinded>                       \
  using if_ ## Abbrev                                                   \
    = typename std::enable_if<is_ ## Abbrev<Kinded>::value, R>::type

  DEFINE(lal, letters);
  DEFINE(lan, nullable);
  DEFINE(lao, one);
  DEFINE(law, words);

# undef DEFINE
}

#endif // !VCSN_CORE_KIND_HH
