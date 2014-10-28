#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <istream>
# include <stdexcept>
# include <type_traits>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /// Define the kinds, and auxiliary tools.
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
  /// context, but then, instead of "(if_lal<Ctx, letter_t> v)",
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
                                                                        \
    static void make(std::istream& is)                                  \
    {                                                                   \
      /*                                                                \
       * name: lal_char(abc), ratexpset<law_char(xyz), b>.              \
       *       ^^^ ^^^^ ^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^                \
       *        |   |    |        weightset                             \
       *        |   |    +-- gens                                       \
       *        |   +-- letter_type                                     \
       *        +-- kind                                                \
       *                                                                \
       * name: lao, ratexpset<law_char(xyz), b>                         \
       *       ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^^                         \
       *       kind         weightset                                   \
       */                                                               \
      char kind[4];                                                     \
      is.get(kind, sizeof kind);                                        \
      if (sname() != kind)                                              \
        throw std::runtime_error("kind::make: unexpected: "             \
                                 + str_escape(kind)                     \
                                 + ", expected: " + sname());           \
    }                                                                   \
                                                                        \
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
  DEFINE(lar, ratexps);
  DEFINE(lat, tuples);
  DEFINE(law, words);

# undef DEFINE
}

#endif // !VCSN_CORE_KIND_HH
