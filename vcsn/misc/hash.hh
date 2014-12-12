#pragma once

#include <functional> // std::equal_to

namespace std
{

  // http://stackoverflow.com/questions/2590677
  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

}

namespace vcsn
{

  /// This is useful to make hashes with labels or weights as keys
  /// without using non-default constructors; to be used along with
  /// vcsn::hash.
  template <typename ValueSet>
  class equal_to : public std::equal_to<typename ValueSet::value_t>
  {
  public:
    using valueset_t = ValueSet;
    using value_t = typename valueset_t::value_t;

    bool operator()(const value_t& v1, const value_t& v2) const
    {
      return valueset_t::equal(v1, v2);
    }
  };

  /// This is useful to make hashes with labels or weights as keys
  /// without using non-default constructors; to be used along with
  /// vcsn::equal_to.
  template <typename ValueSet>
  class hash
  {
  public:
    using valueset_t = ValueSet;
    using value_t = typename valueset_t::value_t;

    size_t operator()(const value_t& v) const
    {
      return valueset_t::hash(v);
    }

    // Not used, but needed to satisfy the specification.  See for example
    // http://www.cplusplus.com/reference/functional/hash/ .
    using result_type = size_t;
    using argument_type = value_t;
  };

  /// Following the naming convention of Boost.
  ///
  /// Use SFINAE to avoid being too eager.
  template <class T>
  inline auto hash_value(const T& v)
    -> decltype(std::hash<T>{}(v))
  {
    std::hash<T> hasher;
    return hasher(v);
  }
} // namespace vcsn
