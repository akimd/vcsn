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
      for (const auto& s: ss)
        hash_combine(res, s);
      return res;
    }
  };
}

namespace vcsn
{
  /// Whether \a e is member of \a s.
  template <typename T, typename Compare, typename Alloc>
  bool
  has(const std::set<T, Compare, Alloc>& s, const T& e)
    ATTRIBUTE_PURE;

  /// The set of values of a map.
  template <typename Key, typename Value, typename Comp, typename Alloc>
  std::set<typename std::map<Key, Value, Comp, Alloc>::mapped_type>
  image(const std::map<Key, Value, Comp, Alloc>& m);

  /// The intersection of two sets.
  template <typename T, typename Compare, typename Alloc>
  std::set<T, Compare, Alloc>
  intersection(const std::set<T, Compare, Alloc>& set1,
               const std::set<T, Compare, Alloc>& set2);

  /// The set of all the intersections of the sets in \a pset.
  template <typename T, typename Compare, typename Alloc>
  std::set<std::set<T, Compare, Alloc>>
  intersection_closure(std::set<std::set<T, Compare, Alloc>> pset);

  /// The union of two sets.
  template <typename T, typename Compare, typename Alloc>
  std::set<T, Compare, Alloc>
  get_union(const std::set<T, Compare, Alloc>& set1,
            const std::set<T, Compare, Alloc>& set2);

  /// Print with a separator.  Meant to help debugging.
  template <typename T, typename Compare, typename Alloc>
  std::ostream&
  print(const std::set<T, Compare, Alloc>& set, std::ostream& o);

  /// Whether set1 \subset set2.
  template <typename Container1, typename Container2>
  bool subset(const Container1& set1, const Container2& set2)
  ATTRIBUTE_PURE;
}

# include <vcsn/misc/set.hxx>

#endif // !VCSN_MISC_SET_HH
