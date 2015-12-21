#pragma once

#include <istream>

#include <vcsn/misc/stream.hh> // eat
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/type_traits.hh> // std::is_same_t

namespace vcsn
{

  /// Define the kinds, and auxiliary tools.
  ///
  /// is_ABBREV<Kinded>: Whether Kinded has a specific kind_t
  ///
  /// if_ABBREV<Kinded, R = Kinded>:
  /// Provide "overloading" on parameter types, for instance to require
  /// letter_t when labels_are_letters, and word_t when
  /// labels_are_words.  See expressionset::atom and expressionset::atom_ for
  /// an example.
  ///
  /// It is very tempting to turn these guys into members of
  /// context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  /// one must write "(typename Cxx::template if_lal<letter_t> v)".

#define DEFINE(Abbrev, Name)                                            \
  struct labels_are_ ## Name                                            \
  {                                                                     \
    static symbol sname()                                               \
    {                                                                   \
      static auto res = symbol{#Abbrev};                                \
      return res;                                                       \
    }                                                                   \
                                                                        \
    static void make(std::istream& is)                                  \
    {                                                                   \
      eat(is, sname());                                                 \
    }                                                                   \
                                                                        \
  };                                                                    \
                                                                        \
  template <typename Kinded>                                            \
  struct is_ ## Abbrev                                                  \
    : std::is_same<typename Kinded::kind_t, labels_are_ ## Name>        \
  {}

  DEFINE(lal, letters);
  DEFINE(lan, nullable);
  DEFINE(lao, one);
  DEFINE(lar, expressions);
  DEFINE(lat, tuples);
  DEFINE(law, words);

#undef DEFINE
}
