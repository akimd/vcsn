#ifndef VCSN_MISC_DYNAMIC_BITSET_HH
# define VCSN_MISC_DYNAMIC_BITSET_HH

// http://stackoverflow.com/questions/12314763/
# define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

# include <boost/dynamic_bitset.hpp>
# include <vcsn/misc/hash.hh>

namespace std
{
  template <>
  struct hash<boost::dynamic_bitset<>>
  {
    size_t operator()(const boost::dynamic_bitset<>& bitset) const
    {
      size_t res = 0;
      for (auto s : bitset.m_bits)
        hash_combine(res, s);
      return res;
    }
  };
}

namespace vcsn
{
  using dynamic_bitset = boost::dynamic_bitset<>;
} // namespace vcsn

#endif // !VCSN_MISC_DYNAMIC_BITSET_HH
