#ifndef VCSN_MISC_NAME_HH
# define VCSN_MISC_NAME_HH

# include <iostream> // std::ostream

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
  DEFINE(int);
  DEFINE(unsigned);

  DEFINE(std::ostream);
#undef DEFINE

  /// A key encoding type strings of \a a and \a b.
  inline
  std::string
  vname(const std::string& a, const std::string& b)
  {
    return a + " x " + b;
  }

  /// A key encoding types of \a a and \a b.
  template <typename A, typename B>
  inline
  std::string
  vname(A& a, B& b)
  {
    return vname(vname(a), vname(b));
  }

} // namespace vcsn

#endif // !VCSN_MISC_NAME_HH
