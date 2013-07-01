#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <type_traits>

namespace vcsn
{
  struct labels_are_letters  {};
  struct labels_are_nullable {};
  struct labels_are_one      {};
  struct labels_are_words    {};


  /*---------------------------------------.
  | Whether Kinded has a specific kind_t.  |
  `---------------------------------------*/

  template <typename Kinded>
  struct is_lal
    : std::is_same<typename Kinded::kind_t, labels_are_letters>::type
  {};

  template <typename Kinded>
  struct is_lan
    : std::is_same<typename Kinded::kind_t, labels_are_nullable>::type
  {};

  template <typename Kinded>
  struct is_lao
    : std::is_same<typename Kinded::kind_t, labels_are_one>::type
  {};

  template <typename Kinded>
  struct is_law
    : std::is_same<typename Kinded::kind_t, labels_are_words>::type
  {};


  // Provide "overloading" on parameter types, for instance to require
  // letter_t when labels_are_letters, and word_t when
  // labels_are_words.  See ratexpset::atom and ratexpset::atom_ for
  // an example.
  //
  // It is very tempting to turns these guys into members of
  // ctx::context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  // one must write "(typename Cxx::template if_lal<letter_t> v)".
  template <typename Kinded, typename R = Kinded>
  using if_lal
    = typename std::enable_if<is_lal<Kinded>::value, R>::type;

  template <typename Kinded, typename R = Kinded>
  using if_lan
    = typename std::enable_if<is_lan<Kinded>::value, R>::type;

  template <typename Kinded, typename R = Kinded>
  using if_lao
    = typename std::enable_if<is_lao<Kinded>::value, R>::type;

  template <typename Kinded, typename R = Kinded>
  using if_law
    = typename std::enable_if<is_law<Kinded>::value, R>::type;

}

#endif // !VCSN_CORE_KIND_HH
