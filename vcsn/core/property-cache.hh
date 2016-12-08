#pragma once

#include <vector>

#include <boost/any.hpp>

#include <vcsn/algos/fwd.hh>          // focus_automaton_impl.
#include <vcsn/concepts/automaton.hh> // Automaton.
#include <vcsn/core/property.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace detail
  {
    /*---------------------.
    | is_focus_automaton.  |
    `---------------------*/

    template <Automaton Aut>
    struct is_focus_automaton : std::false_type {};

    template <std::size_t Tape, Automaton Aut>
    struct is_focus_automaton<detail::focus_automaton_impl<Tape, Aut>>
      : std::true_type {};
  }

  /*-----------------.
  | property_cache.  |
  `-----------------*/

  /// Cache for properties.
  ///
  /// Can store any kind of data using a property tag (see 'property.hh')
  /// as a 'key'.  The data can have a real value or be unknown.
  class property_cache
  {
  public:
    /// The type of the index or id of a property in our cache.
    using index_t = detail::property_indexer::index_t;

    template <typename PropertyTag>
    using prop_t = typename PropertyTag::property_t;

    /*------.
    | get.  |
    `------*/

    /// Retrieve the value of property in the cache, and cast it according
    /// to the defined property type.
    ///
    /// \tparam Aut the automaton which called the function
    /// \param p the tag of a property
    /// \return the cached value for p
    template <Automaton Aut, typename PropertyTag>
    auto get(Aut&, PropertyTag p)
      -> std::enable_if_t<!detail::is_focus_automaton<Aut>{},
                          prop_t<PropertyTag>>
    {
      return boost::any_cast<prop_t<PropertyTag>>(get_any(p));
    }

    /// Retrieve the value of property on a specific tape in the cache,
    /// and cast it according to the defined property type.
    ///
    /// \tparam Aut the automaton which called the function
    /// \tparam PropertyTag the tag of a property
    /// \return the cached value for p.
    template <Automaton Aut, typename PropertyTag>
    auto get(Aut&, PropertyTag)
      -> std::enable_if_t<detail::is_focus_automaton<Aut>{},
                          prop_t<PropertyTag>>
    {
      return boost::any_cast<prop_t<PropertyTag>>(get_any(
            on_tape<Aut::tape, PropertyTag>{}));
    }

    /*------.
    | put.  |
    `------*/

    /// Store a value for the property in the cache. The value's type must be
    /// the same as the one defined for the property.
    ///
    /// \tparam Aut the automaton which called the function
    /// \param p the tag of a property
    /// \param val the value to put in the cache for p
    template <Automaton Aut, typename PropertyTag>
    auto put(Aut&, PropertyTag p, prop_t<PropertyTag> val)
      -> std::enable_if_t<!detail::is_focus_automaton<Aut>{}>
    {
      put(p.id(), val);
    }

    /// Store a value for the property on specific tape in the cache.
    /// The value's type must be the same as the one defined for the property.
    ///
    /// \tparam Aut the automaton which called the function
    /// \tparam PropertyTag the tag of a property
    /// \param val the value to put in the cache for p
    template <Automaton Aut, typename PropertyTag>
    auto put(Aut&, PropertyTag, prop_t<PropertyTag> val)
      -> std::enable_if_t<detail::is_focus_automaton<Aut>{}>
    {
      put(on_tape<Aut::tape, PropertyTag>::id(), val);
    }

    /*-------------.
    | invalidate.  |
    `-------------*/

    /// Invalidate the value in the cache for a property.
    ///
    /// \tparam Aut the automaton which called the function
    /// \param p the tag of a property
    template <Automaton Aut, typename PropertyTag>
    auto invalidate(Aut&, PropertyTag p)
      -> std::enable_if_t<!detail::is_focus_automaton<Aut>{}>
    {
        put(p.id(), boost::any{});
    }

    /// Invalidate the value in the cache for a property on a tape.
    ///
    /// \tparam Aut the automaton which called the function
    /// \tparam PropertyTag the tag of a property
    template <Automaton Aut, typename PropertyTag>
    auto invalidate(Aut&, PropertyTag)
      -> std::enable_if_t<detail::is_focus_automaton<Aut>{}>
    {
        put(on_tape<Aut::tape, PropertyTag>::id(), boost::any{});
    }

    /// Invalidate all properties.
    void invalidate()
    {
      for (auto& p: p_values_)
        p = boost::any{};
    }

    /*-------------.
    | is_unknown.  |
    `-------------*/

    /// Check if there is a defined value for a property.
    ///
    /// \tparam Aut the automaton which called the function
    /// \param p the tag of a property
    /// \return Whether the value of p is known
    template <Automaton Aut, typename PropertyTag>
    auto is_unknown(Aut&, PropertyTag p)
      -> std::enable_if_t<!detail::is_focus_automaton<Aut>{}, bool>
    {
      return get_any(p).empty();
    }

    /// Check if there is a defined value for a property on specific tape.
    ///
    /// \tparam Aut the automaton which called the function
    /// \tparam PropertyTag the tag of a property
    /// \return Whether the value of p is known
    template <Automaton Aut, typename PropertyTag>
    auto is_unknown(Aut&, PropertyTag)
      -> std::enable_if_t<detail::is_focus_automaton<Aut>{}, bool>
    {
      return get_any(on_tape<Aut::tape, PropertyTag>{}).empty();
    }

    /// Update properties for a given function.
    /// What should be done is defined by the `function_prop` of the function
    /// tag (see property.hh). All values can be first invalidated, then
    /// some properties can have their values updated with defined values.
    template <typename FunctionTag>
    void update(FunctionTag)
    {
      if (function_prop<FunctionTag>::invalidate)
        invalidate();
      for (const auto& p: function_prop<FunctionTag>::updated_prop())
        put(p.first, p.second);
    }

    template <Automaton Aut, typename PropertyTag>
    std::ostream& print_prop(Aut& aut, PropertyTag p,
                             std::ostream& o = std::cout)
    {
      o << p.sname() << ": ";
      if (is_unknown(aut, p))
        o << "N/A";
      else
        o << get(aut, p);
      return o;
    }

  private:
    /// Fill the vector with "unknown" values to make the given index valid.
    ///
    /// \param i index of a property which need to valid in the vector
    void fill_prop(index_t i)
    {
      if (p_values_.size() <= i)
        p_values_.resize(i + 1);
    }

    /// Store a value at \a i in the cache, with no guarantee that the
    /// value's type is the same as the one defined for the property at this
    /// index.
    ///
    /// \param i index of a property
    /// \param val value to store at index i
    template <typename T>
    void put(index_t i, T val)
    {
      fill_prop(i);
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      p_values_[i] = boost::any{val};
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
    }

    /// Retrieve the value for a given property under its boost::any form.
    ///
    /// \param p the tag of a property
    /// \return the corresponding boost::any element
    template <typename PropertyTag>
    boost::any& get_any(PropertyTag p)
    {
      auto i = p.id();
      fill_prop(i);
      return p_values_[i];
    }

    /// The properties' values.
    /// Boost::any is used to represent properties' data. It can have a value
    /// (boost::any{val}) or not (boost::any{}). In the last case, the value
    /// is said to be unknown.
    std::vector<boost::any> p_values_;
  };
}
