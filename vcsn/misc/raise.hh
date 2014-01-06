#ifndef VCSN_MISC_RAISE_HH
# define VCSN_MISC_RAISE_HH

# include <stdexcept>
# include <sstream>
# include <utility>

namespace vcsn
{
  namespace detail
  {
    struct pass
    {
      template<typename ...T> pass(T...) {}
    };
  }

  template <typename... Args>
  inline void raise(const Args&... args)
  {
    std::ostringstream o;
    detail::pass{(o << args, 0)...};
    throw std::runtime_error{o.str()};
  }

  template <typename... Args>
  inline void require(bool b, Args&&... args)
  {
    if (!b)
      raise(std::forward<Args>(args)...);
  }
};

#endif // !VCSN_MISC_RAISE_HH
