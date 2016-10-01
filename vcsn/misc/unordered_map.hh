#pragma once

#include <unordered_map>

namespace vcsn
{

  /// Sum of bucket sizes.
  template <typename Key, typename T,
            typename Hash, typename KeyEqual, typename Alloc>
  size_t
  collision_count(const std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& m)
  {
    size_t res = 0;
    for (size_t i = 0; i < m.bucket_count(); ++i)
      res += m.bucket_size(i);
    return res;
  }

  /// Whether \a e is member of \a m.
  template <typename Key, typename T,
            typename Hash, typename KeyEqual, typename Alloc>
  bool
  has(const std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& m,
      const Key& k)
  {
    return m.find(k) != std::end(m);
  }
}
