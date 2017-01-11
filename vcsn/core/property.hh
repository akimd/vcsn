#pragma once

#include <boost/any.hpp>

#include <vcsn/misc/export.hh>
#include <vcsn/misc/property-indexer.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn LIBVCSN_API
{
  /*-------------.
  | Properties.  |
  `-------------*/

  namespace
  {
    symbol property_name(std::string s)
    {
      std::replace(s.begin(), s.end(), '_', ' ');
      return symbol{s};
    }
  }

  /// \def CREATE_PROP(Name, Type)
  /// Define a property. It will have a name, an id (defined at the first call
  /// to it), and a type (to know how to cast its corresponding value when
  /// needed).
  # define CREATE_PROP(Name, Type)                                      \
    struct Name ## _ptag                                                \
    {                                                                   \
      using property_t = Type;                                          \
      using index_t = detail::property_indexer::index_t;                \
                                                                        \
      static symbol& sname()                                            \
      {                                                                 \
        static auto sname = symbol{property_name(#Name)};               \
        return sname;                                                   \
      }                                                                 \
                                                                        \
      static index_t id()                                               \
      {                                                                 \
        static auto id = detail::property_indexer::id();                \
        return id;                                                      \
      }                                                                 \
    };

  CREATE_PROP(is_proper, bool);
  CREATE_PROP(is_deterministic, bool);
  CREATE_PROP(is_free, bool);

  /*----------.
  | on_tape.  |
  `----------*/

  /// This tag is used to represent a property on a specific tape of an
  /// automaton. This is mainly used with focus_automaton.
  ///
  /// \tparam Tape the number of the focused tape
  /// \tparam PropertyTag the tag of a property
  template <std::size_t Tape, typename PropertyTag>
  struct on_tape
  {
    using property_t = typename PropertyTag::property_t;
    using index_t = detail::property_indexer::index_t;

    static symbol& sname()
    {
      static auto sname = symbol{"on_tape<" + std::to_string(Tape) + ", "
                                 + PropertyTag::sname().get() + '>'};
      return sname;
    }

    static index_t id()
    {
      static auto id = detail::property_indexer::id();
      return id;
    }
  };
}
