#pragma once

#include <boost/range/iterator_range_core.hpp>

#include <algorithm>
#include <iterator> // next

namespace vcsn
{
  namespace detail
  {
    // Not in Boost 1.49.
    template <typename Range, typename Predicate>
    bool any_of(const Range &r, Predicate p)
    {
      using std::begin;
      using std::end;
      return std::any_of(begin(r), end(r), p);
    }

    /// The last member of this Container.
    ///
    /// Should specialized with containers with random access.
    template <typename Container>
    typename Container::value_type
    back(const Container& container)
    {
      using std::begin;
      using std::end;
      auto i = begin(container);
      auto iend = end(container);
      assert(i != iend);
      auto res = *i;
      for (++i; i != iend; ++i)
        res = *i;
      return res;
    }

    template< typename Container, typename Predicate>
    void erase_if(Container& c, const Predicate& p)
    {
      using std::begin;
      using std::end;
      for (auto i = begin(c); i != end(c); /* nothing. */)
        {
          if (p(*i))
            i = c.erase(i);
          else
            ++i;
        }
    }


    /// The first member of this Container.
    template <typename Container>
    typename Container::value_type
    front(const Container& container)
    {
      using std::begin;
      using std::end;
      auto i = begin(container);
      assert(i != end(container));
      return *i;
    }

    /// The return the longest initial range of elements matching the
    /// predicate.
    template <typename Iterator, typename Pred, typename Less>
    boost::iterator_range<Iterator>
    initial_sorted_range(Iterator begin, Iterator end,
                         Pred pred, Less less)
    {
      if (pred(*begin))
        for (auto i = begin;; ++i)
          {
            auto next = std::next(i);
            if (next == end
                || !pred(*next)
                || less(*next, *i))
              return {begin, next};
          }
      else
        return {end, end};
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

    // Boost 1.49 does not have boost/algorithm/cxx11/none_of.hpp and the like.
    template <typename Range, typename Predicate>
    bool none_of(const Range &r, Predicate p)
    {
      using std::begin;
      using std::end;
      return std::none_of(begin(r), end(r), p);
    }

    // Not in Boost 1.49.
    template <typename Range, typename Value>
    bool none_of_equal(const Range &r, const Value& value)
    {
      return none_of(r, [&value](const Value& v) { return v == value; });
    }
  }

  /// Check that two associative containers have the same keys.
  template <typename Container>
  bool
  same_domain(const Container& x, const Container& y)
  {
    if (x.size() == y.size())
      {
        using std::begin; using std::end;
        for (auto xi = begin(x), xend = end(x), yi = begin(y);
             xi != xend;
             ++xi, ++yi)
          if (x.key_comp()(xi->first, yi->first)
              || x.key_comp()(yi->first, xi->first))
            return false;
        return true;
      }
    else
      return false;
  }
}
