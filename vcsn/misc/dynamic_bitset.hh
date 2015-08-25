#pragma once

// http://stackoverflow.com/questions/3896357/
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include <boost/dynamic_bitset.hpp>

#include <boost/functional/hash.hpp>

#include <vcsn/misc/functional.hh>

namespace std
{
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
}

namespace vcsn
{
  using dynamic_bitset = boost::dynamic_bitset<>;
}
