#pragma once

#include <map>
#include <set>

#include <vcsn/misc/functional.hh>

namespace std
{

  /*------------------------.
  | hash(map<Key, Value>).  |
  `------------------------*/

  template <typename Key, typename Value, typename Compare, typename Alloc>
  struct hash<map<Key, Value, Compare, Alloc>>
  {
    size_t operator()(const map<Key, Value, Compare, Alloc>& m) const
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

namespace vcsn
{

  /// Whether \a e is in \a s.
  template <typename Key, typename Value, typename Compare, typename Alloc>
  inline
  bool
  has(const std::map<Key, Value, Compare, Alloc>& s, const Key& e)
  {
    return s.find(e) != std::end(s);
  }

  /// The set of values of a map.
  template <typename Key, typename Value, typename Comp, typename Alloc>
  std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type>
  image(const std::map<Key, Value, Comp, Alloc>& m)
  {
    std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type> res;
    for (const auto& p: m)
      res.insert(p.second);
    return res;
  }
}
