#include <iostream>

namespace vcsn
{
  template <typename T>
  inline
  bool
  has(const std::set<T>& s, const T& e)
  {
    return s.find(e) != std::end(s);
  }

  template <typename Key, typename Value, typename Comp, typename Alloc>
  inline
  std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type>
  image(const std::map<Key, Value, Comp, Alloc>& m)
  {
    std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type> res;
    for (const auto& p: m)
      res.insert(p.second);
    return res;
  }


  template <typename T>
  inline
  std::set<T>
  intersection(const std::set<T>& set1, const std::set<T>& set2)
  {
    std::set<T> res;
    std::insert_iterator<std::set<T>> i{res, begin(res)};
    std::set_intersection(begin(set1), end(set1),
                          begin(set2), end(set2),
                          i);
    return res;
  }


  template <typename T>
  inline
  std::set<std::set<T>>
  intersection_closure(std::set<std::set<T>> pset)
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



  template <typename T>
  inline
  std::set<T>
  get_union(const std::set<T>& set1, const std::set<T>& set2)
  {
    std::set<T> res;
    std::insert_iterator<std::set<T>> i{res, begin(res)};
    std::set_union(begin(set1), end(set1),
                          begin(set2), end(set2),
                          i);
    return res;
  }

  template <typename T>
  inline
  std::ostream&
  print(const std::set<T>& set, std::ostream& o)
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
