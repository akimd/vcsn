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


namespace vcsn
{

  /*---------------------------------.
  | make_ordered_pair(e1<T>, e2<T>). |
  `---------------------------------*/

  template <typename T>
  std::pair<T, T> make_ordered_pair(T e1, T e2)
  {
    return e1 < e2 ? std::make_pair(e1, e2) : std::make_pair(e2, e1);
  }
}

#endif // !VCSN_MISC_PAIR_HH
