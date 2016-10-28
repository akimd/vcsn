#pragma once

#include <vector>

#include <boost/any.hpp>

#include <vcsn/core/property.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  /*------------------.
  | property_cache.   |
  `------------------*/

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

    /// Retrieve the value of property in the cache, and cast it according
    /// to the defined property type.
    ///
    /// \param p the tag of a property
    /// \return the cached value for p
    template <typename PropertyTag>
    auto get(PropertyTag p)
    {
      return boost::any_cast<prop_t<PropertyTag>>(get_any(p));
    }

    /// Store a value for the property in the cache. The value's type must be
    /// the same as the one defined for the property.
    ///
    /// \param p the tag of a property
    /// \param val the value to put in the cache for p
    template <typename PropertyTag>
    void put(PropertyTag p, prop_t<PropertyTag> val)
    {
      put(p.id(), val);
    }

    /// Invalidate the value in the cache for a property.
    ///
    /// \param p the tag of a property
    template <typename PropertyTag>
    void invalidate(PropertyTag p)
    {
        put(p.id(), boost::any{});
    }

    /// Invalidate all properties.
    void invalidate()
    {
      for (auto& p: p_values_)
        p = boost::any{};
    }

    /// Check if there is a real value for a property.
    ///
    /// \param p the tag of a property
    /// \return Whether the value of p is known
    template <typename PropertyTag>
    bool is_unknown(PropertyTag p)
    {
      return get_any(p).empty();
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

    template <typename PropertyTag>
    std::ostream& print_prop(PropertyTag p, std::ostream& o = std::cout)
    {
      o << p.sname() << ": ";
      if (is_unknown(p))
        o << "N/A";
      else
        o << get(p);
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
