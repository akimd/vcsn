#pragma once

#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/// Display S and its value.
#define V(S)                                   \
  #S ": " << S << ' '

/// Display S and its value in pseudo XML tags, in case of nesting.
#define VV(S)                                                          \
  "<" #S ":" __HERE__ << '>' << S << "</" << #S << '>'

#define SHOW(S) std::cerr << S << std::endl

#define SHOWV(S) SHOW(V(S))

#define __HERE__ __FILE__ ":" << __LINE__

#define SHOWH(S) SHOW(__HERE__ << ": " << S)

namespace std
{
  template <typename T, typename Compare, typename Alloc>
  ostream&
  operator<<(ostream& o, set<T, Compare, Alloc>& ts)
  {
    bool first = true;
    for (const auto& t: ts)
      {
        if (!first)
          o << ' ';
        o << t;
        first = false;
      }
    return o;
  }

  template <typename Key,  typename T,
            typename Hash, typename KeyEqual, typename Alloc>
  ostream&
  operator<<(ostream& o, unordered_map<Key, T, Hash, KeyEqual, Alloc>& ts)
  {
    bool first = true;
    for (const auto& t: ts)
      {
        if (!first)
          o << ' ';
        o << t.first << ":" << t.second;
        first = false;
      }
    return o;
  }

  template <typename Key, typename Hash, typename KeyEqual, typename Alloc>
  ostream&
  operator<<(ostream& o, unordered_set<Key, Hash, KeyEqual, Alloc>& ts)
  {
    bool first = true;
    for (const auto& t: ts)
      {
        if (!first)
          o << ' ';
        o << t;
        first = false;
      }
    return o;
  }

  template <typename T, typename Alloc>
  ostream&
  operator<<(ostream& o, vector<T, Alloc>& ts)
  {
    bool first = true;
    for (const auto& t: ts)
      {
        if (!first)
          o << ' ';
        o << t;
        first = false;
      }
    return o;
  }
}
