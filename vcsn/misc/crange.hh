#pragma once

#include <type_traits>

#include <boost/iterator/filter_iterator.hpp>

#include <vcsn/misc/attributes.hh>

namespace vcsn
{
  /// Restrict the interface of a container to begin/end.
  template <typename C>
  struct container_range
  {
  public:
    using unref_C = typename std::remove_reference<C>::type;
    using value_type = typename unref_C::value_type;

    using const_iterator = typename unref_C::const_iterator;
    using iterator = typename unref_C::const_iterator;

  private:
    const C cont_;
  public:
    container_range(const unref_C& cont)
      : cont_(cont)
    {}

    /// Forward function Name to the container.
#define DEFINE(Name)                                    \
    auto Name() const -> decltype(this->cont_.Name())   \
    {                                                   \
      return cont_.Name();                              \
    }
    DEFINE(begin);
    DEFINE(end);
    /// The first element.
    /// \pre !empty().
    DEFINE(front);
    /// The last element.
    /// \pre !empty().
    DEFINE(back);
    DEFINE(empty);
    DEFINE(size);
#undef DEFINE
  };


  template <typename Cont, typename Pred>
  struct container_filter_range
  {
  public:
    using container_t = Cont;
    using unref_C = typename std::remove_reference<container_t>::type;
    using value_type = typename unref_C::value_type;
    using predicate_t = Pred;
    using const_iterator
      = boost::filter_iterator<predicate_t, typename unref_C::const_iterator>;
  public:
    container_filter_range(const unref_C& cont, predicate_t predicate)
      : cont_(cont)
      , predicate_(predicate)
    {}

    const_iterator begin() const
    {
      return {predicate_, cont_.begin(), cont_.end()};
    }

    const_iterator end() const
    {
      return {predicate_, cont_.end(), cont_.end()};
    }

    /// The first element.
    /// \pre !empty().
    /// \note: we don't return by reference, as begin returns
    /// a temporary.
    value_type front() const
    {
      assert(!empty());
      return *begin();
    }

    /// The last element.
    /// \pre !empty().
    /// \note: we don't return by reference, as begin returns
    /// a temporary.
    value_type back() const
    {
      assert(!empty());
      return cont_.back();
    }

    ATTRIBUTE_PURE
    bool empty() const
    {
      return begin() == end();
    }

    ATTRIBUTE_PURE
    size_t size() const
    {
      return std::distance(begin(), end());
    }

  private:
    const container_t cont_;
    predicate_t predicate_;
  };

  template <typename Cont, typename Pred>
  container_filter_range<Cont, Pred>
  make_container_filter_range(const Cont& cont, Pred pred)
  {
    return {cont, pred};
  }
}
