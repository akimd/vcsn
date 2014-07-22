#ifndef VCSN_MISC_UNORDERED_MAP_HH
# define VCSN_MISC_UNORDERED_MAP_HH

# include <unordered_map>

namespace vcsn
{

  /// Sum of bucket sizes.
  template <typename Key, typename T,
            typename Hash, typename KeyEqual, typename Alloc>
  inline
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
  inline
  bool
  has(const std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& m,
      const Key& k)
  {
    return m.find(k) != std::end(m);
  }

}

#endif // !VCSN_MISC_UNORDERED_MAP_HH
