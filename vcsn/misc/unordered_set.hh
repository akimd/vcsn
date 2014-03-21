#ifndef VCSN_MISC_UNORDERED_SET_HH
# define VCSN_MISC_UNORDERED_SET_HH

# include <unordered_set>

# include <vcsn/misc/hash.hh>

namespace std
{

  /*-------------------------.
  | hash(unordered_set<T>).  |
  `-------------------------*/

  template <typename T>
  struct hash<unordered_set<T>>
  {
    size_t operator()(const unordered_set<T>& ss) const
    {
      std::hash<T> hasher;
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

}

#endif // !VCSN_MISC_UNORDERED_SET_HH
