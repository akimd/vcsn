#ifndef VCSN_MISC_ALGORITHM_HH
# define VCSN_MISC_ALGORITHM_HH

# include <algorithm>

namespace vcsn
{
  namespace detail
  {
    template <typename Container, typename Compare>
    bool is_sorted(const Container& container, Compare comp)
    {
      return std::is_sorted(std::begin(container),
                            std::end(container),
                            comp);
    }
  }
}

#endif // !VCSN_MISC_ALGORITHM_HH
