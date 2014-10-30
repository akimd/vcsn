#ifndef VCSN_MISC_UNORDERED_SET_HH
# define VCSN_MISC_UNORDERED_SET_HH

# include <unordered_set>

# include <vcsn/misc/hash.hh>

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

}

#endif // !VCSN_MISC_UNORDERED_SET_HH
