#pragma once

#include <algorithm>
#include <iterator> // next
#include <vector>

#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/range/iterator_range_core.hpp>

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

    template <typename Range>
    bool any_of(const Range &r)
    {
      using std::begin;
      using std::end;
      return std::any_of(begin(r), end(r),
                         [](const auto& p) -> bool { return p; });
    }

    /// The last member of this Container.
    ///
    /// Should be specialized for containers with random access.
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

    template <typename Container, typename Predicate>
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

    /// The longest initial range of elements matching the predicate.
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

    /// Lexicographical three-way comparison between two ranges.
    /// \param first1  the beginning of the first range
    /// \param last1   the end of the first range
    /// \param first2  the beginning of the second range
    /// \param last2   the end of the second range
    /// \param comp    a three-way comparison operator
    template <typename InputIt1, typename InputIt2, class Compare>
    int lexicographical_cmp(InputIt1 first1, InputIt1 last1,
                            InputIt2 first2, InputIt2 last2,
                            Compare comp)
    {
      for ( ; first1 != last1 && first2 != last2; ++first1, ++first2)
        if (auto res = comp(*first1, *first2))
          return res;
      // f1 == l1 && f2 != l2 -> -1
      // f1 == l1 && f2 == l2 ->  0
      // f1 != l1 && f2 == l2 -> +1
      return (first1 != last1) - (first2 != last2);
    }

    /// Lexicographical three-way comparison between two ranges.
    /// \param cont1  a range
    /// \param cont2  another range
    /// \param comp   a three-way comparison operator
    template <typename Cont1, typename Cont2, typename Compare>
    int
    lexicographical_cmp(const Cont1& cont1,
                        const Cont2& cont2,
                        Compare comp)
    {
      // FIXME: clang 3.5 does not feature cbegin/cend.
      using std::begin;
      using std::end;
      return lexicographical_cmp(begin(cont1), end(cont1),
                                 begin(cont2), end(cont2),
                                 comp);
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


    /// Map a unary function on a container of values, and return the
    /// vector the results.
    template <typename Container, typename Fun>
    auto transform(const Container& c, Fun&& fun)
    {
      using value_t = decltype(std::forward<Fun>(fun)(*begin(c)));
      auto res = std::vector<value_t>(c.size());
      std::transform(begin(c), end(c), begin(res), std::forward<Fun>(fun));
      return res;
    }
  }

  /// The set of members of \a s1 that are not members of s2.
  template <typename Container,
            // SFINAE.
            typename = typename Container::value_type>
  Container
  set_difference(const Container& s1, const Container& s2)
  {
    auto res = Container{s1.key_comp(), s1.get_allocator()};
    auto i = std::insert_iterator<Container>{res, res.begin()};
    boost::set_difference(s1, s2, i, s1.key_comp());
    return res;
  }

  /// The intersection of two sets.
  template <typename Container,
            // SFINAE.
            typename = typename Container::value_type>
  Container
  set_intersection(const Container& s1, const Container& s2)
  {
    auto res = Container{s1.key_comp(), s1.get_allocator()};
    auto i = std::insert_iterator<Container>{res, res.begin()};
    boost::set_intersection(s1, s2, i, s1.key_comp());
    return res;
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

  /// The union of two sets.
  template <typename Container,
            // SFINAE.
            typename = typename Container::value_type>
  Container
  set_union(const Container& s1, const Container& s2)
  {
    auto res = Container{s1.key_comp(), s1.get_allocator()};
    auto i = std::insert_iterator<Container>{res, res.begin()};
    boost::set_union(s1, s2, i, s1.key_comp());
    return res;
  }
}
