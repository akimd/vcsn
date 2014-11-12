#ifndef VCSN_MISC_NAME_HH
# define VCSN_MISC_NAME_HH

# include <initializer_list>
# include <iostream>
# include <memory>
# include <string>
# include <vector>

# include <vcsn/core/rat/identities.hh>
# include <vcsn/misc/direction.hh>
# include <vcsn/misc/signature.hh>
# include <vcsn/misc/symbol.hh>

namespace vcsn
{

  /*---------------.
  | Static names.  |
  `---------------*/

  template <typename T>
  struct snamer
  {
    symbol operator()()
    {
      return T::sname();
    }
  };

  template <typename T>
  symbol sname()
  {
    return snamer<T>()();
  }

  template <typename T>
  symbol sname(T&)
  {
    return sname<T>();
  }

  /// Static signature.
  template <typename... Args>
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
    symbol operator()(T& t)
    {
      return t->vname();
    }
  };

  template <typename T>
  symbol vname(T& t)
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
    symbol operator()()                         \
    {                                           \
      symbol res(#Type);                        \
      return res;                               \
    }                                           \
  };                                            \
                                                \
  template <>                                   \
  struct vnamer<Type>                           \
  {                                             \
    symbol operator()(Type&)                    \
    {                                           \
      symbol res(#Type);                        \
      return res;                               \
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

  DEFINE(vcsn::rat::identities);
  DEFINE(vcsn::direction);
#undef DEFINE


  /*--------------------.
  | integral_constant.  |
  `--------------------*/


  template <typename T, T Value>
  struct snamer<std::integral_constant<T, Value>>
  {
    symbol operator()()
    {
      symbol res("std::integral_constant<unsigned, "
                 + std::to_string(Value) + '>');
      return res;
    }
  };

  template <typename T, T Value>
  struct vnamer<std::integral_constant<T, Value>>
  {
    using type = std::integral_constant<T, Value>;
    symbol operator()(type)
    {
      return sname<type>();
    }
  };

  /// A simple placeholder for integral constants.
  ///
  /// Consider the case of `blind(automaton, tape)`: it must turn the
  /// `tape` (runtime) argument into a (compile time) parameter.
  ///
  /// The compile-time parameter as a value is painful, as it is an
  /// exception, it is a nuisance for perfect forwarding which is
  /// tailoyred for type parameters (not value parameters), etc. so it
  /// is turned into a type parameter thanks to
  /// std::integral_constant.
  ///
  /// Now the tricky part is therefore going from a runtime integer to
  /// a std::integral_constant.  This is done by having dyn::blind
  /// turn its "unsigned tape" argument into a "integral_constant
  /// tape" one, and the latter, when queried by the signature
  /// extraction mechanism, must simply say "I'm
  /// std::integral_constant<unsigned, tape>".
  struct integral_constant
  {
    symbol name;
  };

  template <>
  struct vnamer<integral_constant>
  {
    symbol operator()(integral_constant t)
    {
      return t.name;
    }
  };


  /*-------------.
  | vsignature.  |
  `-------------*/

  /// The signature of (Args...).
  template <typename... Args>
  inline
  signature
  vsignature(Args&&... args)
  {
    return {vname(std::forward<Args>(args))...};
  }

} // namespace vcsn

#endif // !VCSN_MISC_NAME_HH
