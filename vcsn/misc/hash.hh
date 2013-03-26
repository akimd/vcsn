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

}

#endif // !VCSN_MISC_HASH_HH
