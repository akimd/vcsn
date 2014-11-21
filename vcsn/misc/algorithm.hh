#ifndef VCSN_MISC_ALGORITHM_HH
# define VCSN_MISC_ALGORITHM_HH

# include <algorithm>

namespace vcsn
{
  namespace detail
  {
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

    /// Same as *std::max_element, but works with an input iterator, not
    /// just a forward iterator.
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
  }
}

#endif // !VCSN_MISC_ALGORITHM_HH
