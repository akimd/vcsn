#pragma once

#include <map>
#include <vcsn/misc/hash.hh>

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

  /// Functor to compare Values of ValueSets.
  template <typename ValueSet>
  class less : public std::less<typename ValueSet::value_t>
  {
  public:
    using valueset_t = ValueSet;
    using value_t = typename valueset_t::value_t;

    bool operator()(const value_t& lhs, const value_t& rhs) const
    {
      return valueset_t::less(lhs, rhs);
    }
  };

}
