#pragma once

#include <vcsn/misc/functional.hh> // hash_combine

namespace std
{

  /*-----------------.
  | hash(pair<T>).   |
  `-----------------*/

  template <typename T1, typename T2>
  struct hash<pair<T1, T2>>
  {
    size_t operator()(const pair<T1, T2>& p) const
    {
      size_t res = 0;
      vcsn::hash_combine(res, p.first);
      vcsn::hash_combine(res, p.second);
      return res;
    }
  };
}


namespace vcsn
{

  /*-----------------------------.
  | make_unordered_pair(e1, e2). |
  `-----------------------------*/

  template <typename T>
  auto make_unordered_pair(T&& e1, T&& e2)
  {
    return (e1 < e2
            ? std::make_pair(std::forward<T&&>(e1), std::forward<T&&>(e2))
            : std::make_pair(std::forward<T&&>(e2), std::forward<T&&>(e1)));
  }
}
