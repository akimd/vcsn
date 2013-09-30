#ifndef VCSN_MISC_SET_HH
# define VCSN_MISC_SET_HH

# include <algorithm> // set_intersection
# include <map>
# include <set>

# include <vcsn/misc/attributes.hh>
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

namespace vcsn
{
  /// Whether \a e is member of \a s.
  template <typename T>
  bool
  has(const std::set<T>& s, const T& e)
    ATTRIBUTE_PURE;

  /// The set of values of a map.
  template <typename Key, typename Value, typename Comp, typename Alloc>
  std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type>
  image(const std::map<Key, Value, Comp, Alloc>& m);

  /// The intersection of two sets.
  template <typename T>
  std::set<T>
  intersection(const std::set<T>& set1, const std::set<T>& set2);

  /// The set of all the intersections of the sets in \a pset.
  template <typename T>
  std::set<std::set<T>>
  intersection_closure(std::set<std::set<T>> pset);

  /// The union of two sets.
  template <typename T>
  std::set<T>
  get_union(const std::set<T>& set1, const std::set<T>& set2);

  /// Whether set1 \subset set2.
  template <typename Container1, typename Container2>
  bool subset(const Container1& set1, const Container2& set2)
  ATTRIBUTE_PURE;

}

# include <vcsn/misc/set.hxx>

#endif // !VCSN_MISC_SET_HH
