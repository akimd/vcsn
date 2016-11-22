#pragma once

#include <boost/any.hpp>
#include <boost/container/flat_map.hpp>

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

  /*-----------.
  | Function.  |
  `-----------*/

  /// \def CREATE_FUNCTION_TAG(Name)
  /// Define a function tag, with a name.
  # define CREATE_FUNCTION_TAG(Name)                                    \
    struct LIBVCSN_API Name ## _ftag                                    \
    {                                                                   \
      static symbol& sname()                                            \
      {                                                                 \
        static auto sname = symbol{#Name};                              \
        return sname;                                                   \
      }                                                                 \
    };

  /*----------------------.
  | Function properties.  |
  `----------------------*/

  /// Map containing each property value that a given function will define.
  using updated_prop_t = boost::container::flat_map<
                         detail::property_indexer::index_t, boost::any>;

  using updated_init_list_t = std::initializer_list<std::pair<
                              detail::property_indexer::index_t, boost::any>>;

  inline auto create_updated_prop(updated_init_list_t l)
  {
    return updated_prop_t{l.begin(), l.end()};
  }

  namespace detail
  {
    /// General structure of a function_prop which define properties to update
    /// for a given function.
    /// A function can either invalidate all properties or not, and specify
    /// some properties values (which can be empty values to invalidate only
    /// specific ones).
    template <typename FunctionTag, bool B>
    struct f_prop_
    {
      static const bool invalidate = B;

      // Values to put in cache after invalidation (if asked) for specific
      // properties.
      static auto& updated_prop()
      {
        static auto updated = updated_prop_t{};
        return updated;
      }
    };

    template <typename FunctionTag>
    using invalidate_prop = f_prop_<FunctionTag, true>;

    template <typename FunctionTag>
    using keep_prop = f_prop_<FunctionTag, false>;
  }

  /// Define which property changes when a function is applied.
  /// Current default behavior: invalidate everything and define nothing.
  template <typename FunctionTag>
  struct function_prop : detail::invalidate_prop<FunctionTag> {};

  /// \def KEEP_PROPERTIES(Function)
  /// Define that Function will not invalidate everything and will define
  /// nothing.
# define KEEP_PROPERTIES(Function)                    \
  template <>                                         \
  struct function_prop<Function ## _ftag>             \
    : detail::keep_prop<Function ## _ftag>            \
  {}
}
