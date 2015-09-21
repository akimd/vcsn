#pragma once

#include <initializer_list>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <vcsn/core/rat/identities.hh>
// FIXME: I don't like that misc depends on dyn.  Actually, it is
// misc/name.hh which should be elsewhere.
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/symbol.hh>

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

#define DEFINE(...)                             \
  template <>                                   \
  struct snamer<__VA_ARGS__>                    \
  {                                             \
    symbol operator()()                         \
    {                                           \
      auto res = symbol{#__VA_ARGS__};          \
      return res;                               \
    }                                           \
  };                                            \
                                                \
  template <>                                   \
  struct vnamer<__VA_ARGS__>                    \
  {                                             \
    symbol operator()(__VA_ARGS__&)             \
    {                                           \
      auto res = symbol{#__VA_ARGS__};          \
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
  DEFINE(const std::set<std::pair<std::string, std::string>>);
  DEFINE(std::ostream);

  DEFINE(boost::optional<unsigned>);

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
  /// Consider the case of `focus(automaton, tape)`: it must turn the
  /// `tape` (runtime) argument into a (compile time) parameter.
  ///
  /// The compile-time parameter as a value is painful, as it is an
  /// exception, it is a nuisance for perfect forwarding which is
  /// tailoyred for type parameters (not value parameters), etc. so it
  /// is turned into a type parameter thanks to
  /// std::integral_constant.
  ///
  /// Now the tricky part is therefore going from a runtime integer to
  /// a std::integral_constant.  This is done by having dyn::focus
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


  /*--------------.
  | std::tuple.   |
  `--------------*/

  template <typename... Args>
  struct snamer<std::tuple<Args...>>
  {
    template <typename T1>
    static std::string name()
    {
      return sname<T1>();
    }

    template <typename T1, typename T2, typename... Ts>
    static std::string name()
    {
      return sname<T1>() + ", " + name<T2, Ts...>();
    }

    symbol operator()() const
    {
      return symbol{"std::tuple<" + name<Args...>() + '>'};
    }
  };

  template <>
  struct vnamer<const std::vector<dyn::automaton>>
  {
    using type = const std::vector<dyn::automaton>;
    symbol operator()(const type& t) const
    {
      std::string names;
      for (const auto& a: t)
        {
          if (!names.empty())
            names += ", ";
          names += vname(a);
        }
      return symbol{"std::tuple<" + names + '>'};
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
