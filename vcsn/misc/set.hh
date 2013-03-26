#ifndef VCSN_MISC_SET_HH
# define VCSN_MISC_SET_HH

# include <set>

# include <vcsn/misc/hash.hh>

namespace std
{

  /*---------------.
  | hash(set<T>).  |
  `---------------*/

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

}

#endif // !VCSN_MISC_SET_HH
