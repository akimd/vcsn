#ifndef VCSN_MISC_HASH_HH
# define VCSN_MISC_HASH_HH

namespace std
{

  // http://stackoverflow.com/questions/2590677
  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  // Following the naming convention of Boost.
  template <class T>
  inline std::size_t hash_value(const T& v)
  {
    std::hash<T> hasher;
    return hasher(v);
  }

}

#endif // !VCSN_MISC_HASH_HH
