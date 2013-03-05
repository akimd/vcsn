#ifndef VCSN_MISC_HASH_HH
# define VCSN_MISC_HASH_HH

# include <set>
# include <unordered_set>

namespace std
{

  /*---------------.
  | hash(set<T>).  |
  `---------------*/

  // http://stackoverflow.com/questions/2590677
  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  template <typename T>
  struct hash<set<T>>
  {
    size_t operator()(const set<T>& ss) const
    {
      size_t res = 0;
      for (auto s: ss)
        hash_combine(res, s);
      return res;
    }
  };


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

#endif // !VCSN_MISC_HASH_HH
