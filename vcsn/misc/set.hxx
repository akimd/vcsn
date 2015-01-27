#include <iostream>

namespace vcsn
{
  template <typename T, typename Compare, typename Alloc>
  inline
  bool
  has(const std::set<T, Compare, Alloc>& s, const T& e)
  {
    return s.find(e) != std::end(s);
  }


  template <typename T, typename Compare, typename Alloc>
  inline
  std::set<T, Compare, Alloc>
  intersection(const std::set<T, Compare, Alloc>& set1,
               const std::set<T, Compare, Alloc>& set2)
  {
    using set_t = std::set<T, Compare, Alloc>;
    set_t res;
    std::insert_iterator<set_t> i{res, begin(res)};
    std::set_intersection(begin(set1), end(set1),
                          begin(set2), end(set2),
                          i);
    return res;
  }


  template <typename T, typename Compare, typename Alloc>
  inline
  std::set<std::set<T, Compare, Alloc>>
  intersection_closure(std::set<std::set<T, Compare, Alloc>> pset)
  {
    while (true)
      {
        bool done = true;
        for (const auto& set1: pset)
          for (const auto& set2: pset)
            if (pset.emplace(intersection(set1, set2)).second)
              done = false;
        if (done)
          break;
      }
    return pset;
  }


  template <typename T, typename Compare, typename Alloc>
  inline
  std::set<T, Compare, Alloc>
  get_union(const std::set<T, Compare, Alloc>& set1,
            const std::set<T, Compare, Alloc>& set2)
  {
    using set_t = std::set<T, Compare, Alloc>;
    set_t res;
    std::insert_iterator<set_t> i{res, begin(res)};
    std::set_union(begin(set1), end(set1),
                   begin(set2), end(set2),
                   i);
    return res;
  }


  template <typename T, typename Compare, typename Alloc>
  inline
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


  template <typename Container1, typename Container2>
  inline
  bool subset(const Container1& set1, const Container2& set2)
  {
    return std::includes(set2.begin(), set2.end(),
                         set1.begin(), set1.end());
  }
}
