#ifndef VCSN_MISC_MAP_HH
# define VCSN_MISC_MAP_HH

# include <map>
# include <vcsn/misc/hash.hh>

namespace std
{

  /*------------------------.
  | hash(map<Key, Value>).  |
  `------------------------*/

  template <typename Key, typename Value>
  struct hash<map<Key, Value>>
  {
    size_t operator()(const map<Key, Value>& m) const
    {
      size_t res = 0;
      for (const auto& kv: m)
        {
          hash_combine(res, kv.first);
          hash_combine(res, kv.second);
        }
      return res;
    }
  };
}

#endif // !VCSN_MISC_MAP_HH
