#include <iostream>

#include <vcsn/misc/algorithm.hh> // set_intersection

namespace vcsn
{
  template <typename T, typename Compare, typename Alloc>
  bool
  has(const std::set<T, Compare, Alloc>& s, const T& e)
  {
    return s.find(e) != std::end(s);
  }


  template <typename T, typename Compare, typename Alloc>
  std::set<std::set<T, Compare, Alloc>>
  intersection_closure(std::set<std::set<T, Compare, Alloc>> pset)
  {
    while (true)
      {
        bool done = true;
        for (const auto& set1: pset)
          for (const auto& set2: pset)
            if (pset.emplace(set_intersection(set1, set2)).second)
              done = false;
        if (done)
          break;
      }
    return pset;
  }


  template <typename T, typename Compare, typename Alloc>
  std::ostream&
  print(const std::set<T, Compare, Alloc>& set, std::ostream& o)
  {
    const char* sep = "";
    for (const auto& m: set)
      {
        o << sep << m;
        sep = ", ";
      }
    return o;
  }


  template <typename Container>
  bool subset(const Container& set1, const Container& set2)
  {
    return std::includes(set2.begin(), set2.end(),
                         set1.begin(), set1.end(),
                         set1.key_comp());
  }
}
