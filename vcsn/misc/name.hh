#ifndef VCSN_MISC_NAME_HH
# define VCSN_MISC_NAME_HH

# include <initializer_list>
# include <iostream>
# include <memory>
# include <vector>
# include <string>

# include <vcsn/misc/signature.hh>

namespace vcsn
{

  /*---------------.
  | Static names.  |
  `---------------*/

  template <typename T>
  struct snamer
  {
    std::string operator()()
    {
      return T::sname();
    }
  };

  template <typename T>
  std::string sname()
  {
    return snamer<T>()();
  }

  template <typename T>
  std::string sname(T&)
  {
    return sname<T>();
  }

  /// Static signature.
  template <typename ... Args>
  inline
  signature
  ssignature()
  {
    return {sname<Args>()...};
  }

  /*----------------.
  | Dynamic names.  |
  `----------------*/

  template <typename T>
  struct vnamer
  {
    std::string operator()(T& t)
    {
      return t->vname(false);
    }
  };

  template <typename T>
  std::string vname(T& t)
  {
    return vnamer<T>()(t);
  }

  /*------------------.
  | Specializations.  |
  `------------------*/


  template <typename T>
  struct snamer<std::shared_ptr<T>>: snamer<T>
  {};

#define DEFINE(Type)                            \
  template <>                                   \
  struct snamer<Type>                           \
  {                                             \
    std::string operator()()                    \
    {                                           \
      return #Type;                             \
    }                                           \
  };                                            \
                                                \
  template <>                                   \
  struct vnamer<Type>                           \
  {                                             \
    std::string operator()(Type&)               \
    {                                           \
      return #Type;                             \
    }                                           \
  };


  DEFINE(bool);
  DEFINE(float);
  DEFINE(int);
  DEFINE(unsigned);

  DEFINE(std::istream);
  DEFINE(const std::string);
  DEFINE(const std::vector<unsigned>);
  DEFINE(std::ostream);
#undef DEFINE

  /// The signature of (Args...).
  template <typename ... Args>
  inline
  signature
  vsignature(Args&&... args)
  {
    return {vname(std::forward<Args>(args))...};
  }

} // namespace vcsn

#endif // !VCSN_MISC_NAME_HH
