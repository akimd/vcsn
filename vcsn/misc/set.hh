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
  bool
  has(const std::set<T, Compare, Alloc>& s, const T& e)
    ATTRIBUTE_PURE;

  /// The set of all the intersections of the sets in \a pset.
  template <typename T, typename Compare, typename Alloc>
  std::set<std::set<T, Compare, Alloc>>
  intersection_closure(std::set<std::set<T, Compare, Alloc>> pset);

  /// Print with a separator.  Meant to help debugging.
  template <typename T, typename Compare, typename Alloc>
  std::ostream&
  print(const std::set<T, Compare, Alloc>& set, std::ostream& o);

  /// Whether \a set1 ⊆ \a set2.
  template <typename Container>
  bool subset(const Container& set1, const Container& set2)
  ATTRIBUTE_PURE;
}

#include <vcsn/misc/set.hxx>
