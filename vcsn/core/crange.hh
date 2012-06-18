#ifndef VCSN_CORE_CRANGE_HH
# define VCSN_CORE_CRANGE_HH

# include <boost/iterator/filter_iterator.hpp>
# include <type_traits>

namespace vcsn
{
  // Restrict the interface of a container to begin/end.
  template <class C>
  struct container_range
  {
  public:
    using unref_C = typename std::remove_reference<C>::type;
    using value_type = typename unref_C::value_type;

    using const_iterator = typename unref_C::const_iterator;
  private:
    const C cont_;
  public:
    container_range(const unref_C& cont) : cont_(cont) {}

    /// Forward function Name to the container.
#define DEFINE(Name)                                    \
    auto Name() const -> decltype(this->cont_.Name())   \
    {                                                   \
      return cont_.Name();                              \
    }
    DEFINE(begin);
    DEFINE(end);
    DEFINE(empty);
    DEFINE(size);
#undef DEFINE
  };


  template <class C>
  struct container_filter_range
  {
  public:
    using unref_C = typename std::remove_reference<C>::type;
    using value_type = typename unref_C::value_type;
    using predicate_t = std::function<bool(value_type)>;
    using const_iterator
      = boost::filter_iterator<predicate_t, typename unref_C::const_iterator>;
  public:
    container_filter_range(const unref_C& cont, predicate_t predicate)
      : cont_(cont), predicate_(predicate)
    {}

    const_iterator begin() const
    {
      return const_iterator(predicate_, cont_.begin(), cont_.end());
    }

    const_iterator end() const
    {
      return const_iterator(predicate_, cont_.end(), cont_.end());
    }

    bool empty() const
    {
      return begin() == end();
    }

    size_t size() const
    {
      return std::distance(begin(), end());
    }

  private:
    const C cont_;
    predicate_t predicate_;
  };
}

#endif // !VCSN_CORE_CRANGE_HH
