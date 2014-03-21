#ifndef VCSN_MISC_UNORDERED_MAP_HH
# define VCSN_MISC_UNORDERED_MAP_HH

# include <unordered_map>

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

}

#endif // !VCSN_MISC_UNORDERED_MAP_HH
