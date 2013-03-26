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

#endif // !VCSN_MISC_UNORDERED_SET_HH
