#pragma once

#include <algorithm> // set_intersection
#include <set>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/functional.hh>

namespace std
{

  /*---------------.
  | hash(set<T>).  |
  `---------------*/

  template <typename T, typename Compare, typename Alloc>
  struct hash<set<T, Compare, Alloc>>
  {
    using type = set<T, Compare, Alloc>;
    size_t operator()(const type& ss) const
    {
      size_t res = 0;
      for (const auto& s: ss)
        hash_combine(res, s);
      return res;
    }
  };
}

namespace vcsn
{
  /// Whether \a e is member of \a s.
  template <typename T, typename Compare, typename Alloc>
  ATTRIBUTE_PURE
  bool
  has(const std::set<T, Compare, Alloc>& s, const T& e);

  /// Whether \a e is member of \a s.
  template <typename T, typename Compare, typename Alloc>
  ATTRIBUTE_PURE
  bool
  operator%(const T& e, const std::set<T, Compare, Alloc>& s)
  {
    return has(s, e);
  }

  /// The intersection of two sets.
  template <typename T, typename Compare, typename Alloc>
  std::set<T, Compare, Alloc>
  intersection(const std::set<T, Compare, Alloc>& set1,
               const std::set<T, Compare, Alloc>& set2);

  /// The set of all the intersections of the sets in \a pset.
  template <typename T, typename Compare, typename Alloc>
  std::set<std::set<T, Compare, Alloc>>
  intersection_closure(std::set<std::set<T, Compare, Alloc>> pset);

  /// The union of two sets.
  template <typename T, typename Compare, typename Alloc>
  std::set<T, Compare, Alloc>
  get_union(const std::set<T, Compare, Alloc>& set1,
            const std::set<T, Compare, Alloc>& set2);

  /// Print with a separator.  Meant to help debugging.
  template <typename T, typename Compare, typename Alloc>
  std::ostream&
  print(const std::set<T, Compare, Alloc>& set, std::ostream& o);

  /// Whether \a set1 ⊆ \a set2.
  template <typename Container1, typename Container2>
  bool subset(const Container1& set1, const Container2& set2)
  ATTRIBUTE_PURE;
}

#include <vcsn/misc/set.hxx>
