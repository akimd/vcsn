#pragma once

#include <functional> // std::equal_to

namespace vcsn
{
  namespace detail
  {
    /// Functor to three-way comparison Values of ValueSets.
    template <typename ValueSet, typename Value = typename ValueSet::value_t>
    struct compare
    {
      using valueset_t = ValueSet;
      using value_t = Value;

      int operator()(const value_t& lhs, const value_t& rhs) const
      {
        return valueset_t::compare(lhs, rhs);
      }
    };
  }

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

  namespace detail
  {
    /// Following the naming convention of Boost.
    ///
    /// Use SFINAE to avoid being too eager.
    template <typename T>
    auto hash_value(const T& v)
      -> decltype(std::hash<T>{}(v))
    {
      auto hasher = std::hash<T>{};
      return hasher(v);
    }
  }

  using detail::hash_value;

  // http://stackoverflow.com/questions/2590677
  inline void hash_combine_hash(std::size_t& seed, size_t h)
  {
    seed ^= h + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  // http://stackoverflow.com/questions/2590677
  template <typename T>
  void hash_combine(std::size_t& seed, const T& v)
  {
    hash_combine_hash(seed, hash_value(v));
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

  /// Functor to compare Values of ValueSets.
  template <typename ValueSet, typename Value = typename ValueSet::value_t>
  struct less_equal
  {
    using valueset_t = ValueSet;
    using value_t = Value;

    bool operator()(const value_t& lhs, const value_t& rhs) const
    {
      return valueset_t::less(lhs, rhs) || valueset_t::equal(lhs, rhs);
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

#if defined __clang__ and __clang_major__ == 3 and __clang_minor__ < 7
  // For clang 3.5, this is needed for ADL
  using vcsn::detail::hash_value;
#endif
};
