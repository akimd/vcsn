#ifndef VCSN_CORE_CRANGE_HH
# define VCSN_CORE_CRANGE_HH

# include <boost/iterator/filter_iterator.hpp>

namespace vcsn
{
  // Restrict the interface of a container to begin/end.
  template <class C>
  struct container_range
  {
  public:
    typedef typename C::value_type value_type;
    typedef typename C::const_iterator const_iterator;
  public:
    container_range(const C& cont) : cont_(cont) {}

    const_iterator begin() const { return cont_.begin(); }
    const_iterator end() const   { return cont_.end(); }
  private:
    const C& cont_;
  };


  template <class C>
  struct container_filter_range
  {
  public:
    typedef typename C::value_type value_type;
    typedef std::function<bool(typename C::value_type)> predicate_t;
    typedef boost::filter_iterator<predicate_t, typename C::const_iterator> const_iterator;
  public:
    container_filter_range(const C& cont, predicate_t predicate)
      : cont_(cont), predicate_(predicate) {}

    const_iterator begin() const
    {
      return const_iterator(predicate_, cont_.begin(), cont_.end());
    }

    const_iterator end() const
    {
      return const_iterator(predicate_, cont_.end(), cont_.end());
    }
  private:
    const C& cont_;
    predicate_t predicate_;
  };
}

#endif // !VCSN_CORE_CRANGE_HH
