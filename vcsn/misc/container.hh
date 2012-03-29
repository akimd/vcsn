#ifndef VCSN_MISC_CONTAINER_HH
# define VCSN_MISC_CONTAINER_HH

namespace vcsn
{

  template <class Container>
  inline
  typename Container::iterator
  last(Container &c)
  {
    return --c.end();
  }


} // namespace vcsn

#endif // !VCSN_MISC_CONTAINER_HH
