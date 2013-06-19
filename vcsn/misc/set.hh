#ifndef VCSN_MISC_SET_HH
# define VCSN_MISC_SET_HH

# include <algorithm> // set_intersection
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

  /// Whether \a e is member of \a s.
  template <typename T>
  bool
  has(const std::set<T>& s, const T& e)
  {
    return s.find(e) != std::end(s);
  }

  /// The set of values of a map.
  template <typename Key, typename Value, typename Comp, typename Alloc>
  std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type>
  image(const std::map<Key, Value, Comp, Alloc>& m)
  {
    std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type> res;
    for (const auto& p: m)
      res.insert(p.second);
    return res;
  }

  /// The intersection of two sets.
  template <typename T>
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

  /// The set of all the intersections of the sets in \a pset.
  template <typename T>
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

  /// Whether set1 \subset set2.
  template <typename Container1, typename Container2>
  bool subset(const Container1& set1, const Container2& set2)
  {
    return std::includes(set2.begin(), set2.end(),
                         set1.begin(), set1.end());
  }

}

#endif // !VCSN_MISC_SET_HH
