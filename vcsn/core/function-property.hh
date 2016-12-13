#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include <vcsn/misc/export.hh>
#include <vcsn/misc/property-indexer.hh>

namespace vcsn LIBVCSN_API
{
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

  /// Contain indices to ignore when invalidating properties.
  using ignored_prop_t = boost::container::flat_set<
                         detail::property_indexer::index_t>;

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

      // Properties to ignore if invalidation is asked.
      static auto& ignored_prop()
      {
        static auto ignored = ignored_prop_t{};
        return ignored;
      }

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

  /*---------------------------.
  | function_prop management.  |
  `---------------------------*/

  namespace
  {
    template <typename FunctionTag, typename PropertyTag, typename T>
    void add_update_(FunctionTag, PropertyTag, T value)
    {
      auto& updated = function_prop<FunctionTag>::updated_prop();
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      // Possibly override previous value for the property.
      updated[PropertyTag::id()] = value;
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
    }
  }

  /// Specify that this function will define this property to a given value.
  /// At the end of the cache update, the property's value is guaranteed to be
  /// the given one.
  ///
  /// \param f the tag of the function
  /// \param p the tag of the property
  /// \param value the value that will be stored after a cache update with f
  template <typename FunctionTag, typename PropertyTag>
  void add_update(FunctionTag f, PropertyTag p,
                  typename PropertyTag::property_t value)
  {
    add_update_(f, p, value);
  }

  /// Invalidate a property when using a given function.
  /// At the end of the cache update, the property's value is guaranteed empty.
  ///
  /// \param f the tag of the function
  /// \param p the tag of the property
  template <typename FunctionTag, typename PropertyTag>
  void add_invalidate(FunctionTag f, PropertyTag p)
  {
    add_update_(f, p, boost::any{});
  }

  /// Preserve the original value of a property when using a given function.
  /// At the end of the cache update, it is guaranteed that the property is
  /// left untouched (neither invalidated nor updated with a given value).
  ///
  /// \tparam FunctionTag the tag of the function
  /// \param p the tag of the property
  template <typename FunctionTag, typename PropertyTag>
  void add_preserve(FunctionTag, PropertyTag p)
  {
    using fp = function_prop<FunctionTag>;

    if (fp::invalidate)
      fp::ignored_prop().emplace(p.id());
    // There could be a defined value for the property, remove it to
    // preserve the original one.
    fp::updated_prop().erase(p.id());
  }

  /// Specify that this function does not define a value for the given property.
  /// At the end of the cache update, the property's value can either be left
  /// untouched or invalidated.
  ///
  /// \tparam FunctionTag the tag of the function
  /// \param p the tag of the property
  template <typename FunctionTag, typename PropertyTag>
  void remove_update(FunctionTag, PropertyTag p)
  {
    function_prop<FunctionTag>::updated_prop().erase(p.id());
  }

  /// Specify that this function does not invalidate the given property.
  /// At the end of the cache update, the property's value can either be left
  /// untouched or defined.
  ///
  /// \tparam FunctionTag the tag of the function
  /// \param p the tag of the property
  template <typename FunctionTag, typename PropertyTag>
  void remove_invalidate(FunctionTag, PropertyTag p)
  {
    using fp = function_prop<FunctionTag>;

    if (fp::invalidate)
      fp::ignored_prop().emplace(p.id());

    // The property could be updated with an empty value.
    auto& updated = fp::updated_prop();
    auto search = updated.find(p.id());
    if (search != updated.end() && search->second.empty())
      updated.erase(search);
  }

  /// Specify that this function does not preserve the given property.
  /// At the end of the cache update, the property's value can either be
  /// defined or invalidated.
  ///
  /// \tparam FunctionTag the tag of the function
  /// \param p the tag of the property
  template <typename FunctionTag, typename PropertyTag>
  void remove_preserve(FunctionTag, PropertyTag p)
  {
    using fp = function_prop<FunctionTag>;

    auto& updated = fp::updated_prop();
    auto search = updated.find(p.id());
    if (search == updated.end())
      updated.emplace(p.id(), boost::any{});
  }

  /// Remove all properties' defined values for a given function.
  template <typename FunctionTag>
  void clear_updated(FunctionTag)
  {
    function_prop<FunctionTag>::updated_prop().clear();
  }

  /// Remove all ignored properties for invalidation for a given function.
  template <typename FunctionTag>
  void clear_ignored(FunctionTag)
  {
    function_prop<FunctionTag>::ignored_prop().clear();
  }
} // namespace vcsn
