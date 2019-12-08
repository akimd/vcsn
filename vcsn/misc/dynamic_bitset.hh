#pragma once

# include <boost/version.hpp>
// http://stackoverflow.com/questions/3896357/
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include <boost/dynamic_bitset.hpp>

#include <boost/functional/hash.hpp>

#include <vcsn/misc/functional.hh>

namespace std
{
#if BOOST_VERSION < 107100
  template <typename B, typename A>
  struct hash<boost::dynamic_bitset<B, A>>
  {
    size_t operator()(const boost::dynamic_bitset<B, A>& bs) const
    {
      size_t res = boost::hash_value(bs.m_num_bits);
      vcsn::hash_combine_hash(res, boost::hash_value(bs.m_bits));
      return res;
    }
  };
#endif

  /// Whether \a e is in \a s.
  template <typename B, typename A, typename Key>
  bool
  has(const boost::dynamic_bitset<B, A>& s, const Key& e)
  {
    return s.test(e);
  }
}

namespace vcsn
{
  using dynamic_bitset = boost::dynamic_bitset<>;

  template <typename Container>
  dynamic_bitset
  make_dynamic_bitset(const Container& c, size_t size)
  {
    auto res = dynamic_bitset(size);
    for (auto s: c)
      res.set(s);
    return res;
  }
}
