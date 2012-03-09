#ifndef VCSN_CORE_CRANGE_HH
#define VCSN_CORE_CRANGE_HH

#include <boost/iterator/filter_iterator.hpp>

namespace vcsn
{
  // Restrict the interface of a container to begin/end.
  template <class C>
  struct container_range
  {
  public:
    container_range(C& cont) : cont_(cont) {}
    typename C::iterator begin() { return cont_.begin(); }
    typename C::iterator end()   { return cont_.end(); }
  private:
    C& cont_;
  };



  template <class C>
  struct container_filter_range
  {
  public:
    typedef std::function<bool(typename C::value_type)> predicate_t;
    typedef boost::filter_iterator<predicate_t, typename C::iterator> iterator_t;
    container_filter_range(C& cont, predicate_t predicate)
      : cont_(cont), predicate_(predicate) {}

    iterator_t begin()
    {
      return iterator_t(predicate_, cont_.begin(), cont_.end());
    }

    iterator_t end()
    {
      return iterator_t(predicate_, cont_.end(), cont_.end());
    }
  private:
    C& cont_;
    predicate_t predicate_;
  };
}

#endif // VCSN_CORE_CRANGE_HH
