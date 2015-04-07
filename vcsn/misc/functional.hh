#pragma once

#include <functional> // std::equal_to

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

  // http://stackoverflow.com/questions/2590677
  inline void hash_combine_hash(std::size_t& seed, size_t h)
  {
    seed ^= h + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  // http://stackoverflow.com/questions/2590677
  template <typename T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    hash_combine_hash(seed, hasher(v));
  }

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
  template <typename T>
  inline auto hash_value(const T& v)
    -> decltype(std::hash<T>{}(v))
  {
    std::hash<T> hasher;
    return hasher(v);
  }

  /// Functor to compare Values of ValueSets.
  template <typename ValueSet, typename Value = typename ValueSet::value_t>
  struct less
  {
    using valueset_t = ValueSet;
    using value_t = Value;

    bool operator()(const value_t& lhs, const value_t& rhs) const
    {
      return valueset_t::less(lhs, rhs);
    }
  };
} // namespace vcsn

namespace std
{
  template <typename Value, size_t Size>
  struct hash<std::array<Value, Size>>
  {
  public:
    using value_t = std::array<Value, Size>;

    size_t operator()(const value_t& v) const
    {
      std::hash<Value> h;
      size_t res = h(v[0]);
      for (size_t i = 1; i < Size; i++)
        vcsn::hash_combine(res, v[i]);
      return res;
    }
  };
};
