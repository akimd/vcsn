#pragma once

#include <algorithm>

namespace vcsn
{
  namespace detail
  {
    /// The last member of this Container.
    ///
    /// Should specialized with containers with random access.
    template <typename Container>
    typename Container::value_type
    back(const Container& container)
    {
      auto i = std::begin(container);
      auto end = std::end(container);
      assert(i != end);
      if (i != end)
        {
          auto res = *i;
          for (++i; i != end; ++i)
            res = *i;
          return res;
        }
      abort();
    }


    /// Same as std::is_sorted, but works with an input iterator, not
    /// just a forward iterator.
    ///
    /// See http://stackoverflow.com/questions/25906893.
    template <typename Container, typename Compare>
    bool is_sorted_forward(const Container& container, Compare comp)
    {
      auto i = std::begin(container);
      auto end = std::end(container);
      if (i != end)
        {
          auto prev = *i;
          for (++i; i != end; ++i)
            if (comp(prev, *i))
              prev = *i;
            else
              return false;
        }
      return true;
    }

    /// Same as \c *std\::max_element, but works with an input iterator,
    /// not just a forward iterator.
    template <typename Container>
    typename Container::value_type
    max_forward(const Container& container)
    {
      auto i = std::begin(container);
      auto end = std::end(container);
      assert(i != end);
      if (i != end)
        {
          auto res = *i;
          for (++i; i != end; ++i)
            if (res < *i)
              res = *i;
          return res;
        }
      abort();
    }

    /// Same as C++14's mismatch, which is not available in G++-4.8
    /// with -std=c++1y.  Get rid of this once we drop G++ 4.8
    /// compatibility.
    template <typename InputIt1, typename InputIt2>
    std::pair<InputIt1, InputIt2>
    mismatch(InputIt1 first1, InputIt1 last1,
             InputIt2 first2, InputIt2 last2)
    {
      while (first1 != last1
             && first2 != last2
             && *first1 == *first2)
        {
          ++first1;
          ++first2;
        }
      return std::make_pair(first1, first2);
    }
  }

  /// Check that two associative containers have the same keys.
  template <typename Container>
  bool
  same_domain(const Container& x, const Container& y)
  {
    if (x.size() != y.size())
      return false;
    // FIXME: no cbegin etc. in C++11.
    using std::begin; using std::end;
    for (auto xi = begin(x), xend = end(x), yi = begin(y);
         xi != xend;
         ++xi, ++yi)
      if (x.key_comp()(xi->first, yi->first)
          || x.key_comp()(yi->first, xi->first))
        return false;
    return true;
  }
}
