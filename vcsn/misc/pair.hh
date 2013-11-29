#ifndef VCSN_MISC_PAIR_HH
# define VCSN_MISC_PAIR_HH

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>

namespace std
{

  /*---------------.
  | hash(pair<T>).  |
  `---------------*/

  template <typename T1, typename T2>
  struct hash<pair<T1, T2>>
  {
    size_t operator()(const pair<T1, T2>& p) const
    {
      size_t res = 0;
      hash_combine(res, p.first);
      hash_combine(res, p.second);
      return res;
    }
  };
}

#endif // !VCSN_MISC_PAIR_HH
