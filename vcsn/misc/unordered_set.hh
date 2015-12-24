#pragma once

#include <unordered_set>

#include <vcsn/misc/functional.hh>

namespace std
{

  /*-------------------------.
  | hash(unordered_set<T>).  |
  `-------------------------*/

  template <typename Key, typename Hash, typename KeyEqual, typename Alloc>
  struct hash<unordered_set<Key, Hash, KeyEqual, Alloc>>
  {
    using type = unordered_set<Key, Hash, KeyEqual, Alloc>;
    size_t operator()(const type& ss) const
    {
      // Compute the sum of the hashes.  Beware that we must be
      // independant of the order, since this is an unordered_set.
      Hash hasher;
      size_t res = 0;
      for (auto s: ss)
        res += hasher(s);
      return res;
    }
  };
}

namespace vcsn
{

  /// Whether \a e is member of \a s.
  template <typename Key, typename Hash, typename KeyEqual, typename Alloc>
  bool
  has(const std::unordered_set<Key, Hash, KeyEqual, Alloc>& s, const Key& k)
  {
    return s.find(k) != std::end(s);
  }

  /// Intersection of \a s1 and \a s2.
  template <typename Key, typename Hash, typename KeyEqual, typename Alloc>
  std::unordered_set<Key, Hash, KeyEqual, Alloc>
  set_intersection(const std::unordered_set<Key, Hash, KeyEqual, Alloc>& s1,
                   const std::unordered_set<Key, Hash, KeyEqual, Alloc>& s2)
  {
    if (s2.size() < s1.size())
      return set_intersection(s2, s1);
    else
      {
        auto res = std::unordered_set<Key, Hash, KeyEqual, Alloc>{};
        for (const auto& e : s1)
          if (has(s2, e))
            res.emplace(e);
        return res;
      }
  }
}
