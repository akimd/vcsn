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

namespace vcsn
{
  template <typename Key, typename Value>
  inline
  bool
  has(const std::map<Key, Value>& s, const Key& e)
  {
    return s.find(e) != std::end(s);
  }

  template <typename ValueSet>
  class less : public std::less<typename ValueSet::value_t>
  {
  public:
    using valueset_t = ValueSet;
    using value_t = typename valueset_t::value_t;

    bool operator()(const value_t& lhs, const value_t& rhs) const
    {
      return valueset_t::less_than(lhs, rhs);
    }
  };

}

#endif // !VCSN_MISC_MAP_HH
