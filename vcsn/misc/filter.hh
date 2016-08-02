#pragma once

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/distance.hpp>

namespace vcsn
{
  namespace detail
  {
    template <typename Range, typename Filter>
    struct safe_filtered
    {
      safe_filtered(Range r, Filter f)
        : range_(std::move(r))
        , filtered_(f, range_)
      {}

      using value_type = typename Range::value_type;
      using difference_type = std::ptrdiff_t;
      using filtered_t = boost::filtered_range<Filter, Range>;

      using iterator = typename filtered_t::iterator;
      using const_iterator = typename filtered_t::const_iterator;

      const_iterator begin() const { return std::begin(filtered_); }
      const_iterator end() const { return std::end(filtered_); }

      /// Whether filters to nothing.
      bool empty() const
      {
        return filtered_.empty();
      }

      /// Number of elements in the filtered range.
      size_t size() const
      {
        return boost::distance(filtered_);
      }

      Range range_;
      filtered_t filtered_;
    };
  }

  /// Similar to boost::adaptors::filter, but ensures that the range
  /// is kept alive.
  template <typename Range, typename Filter>
  auto filter(Range&& r, Filter f,
              // SFINAE against vcsn::filter for automata.
              decltype(f(*std::begin(r)), void())* = nullptr)
  {
    return detail::safe_filtered<Range, Filter>{std::forward<Range>(r), f};
  }
}

namespace boost
{
  template <typename Range, typename Filter>
  struct range_difference<vcsn::detail::safe_filtered<Range, Filter>>
  {
    using type
      = typename vcsn::detail::safe_filtered<Range, Filter>::difference_type;
  };
}
