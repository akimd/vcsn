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
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  // This file provides the basic blocks needed by the implementation
  // of dyn:: bridges.
  //
  // We need two types of signatures.
  //
  // The runtime signatures (vname and vsignature) are obtained during
  // a call to a dyn:: routine: all its arguments are queried to get
  // their runtime type name (e.g., `mutable_automaton<lal_char, b>`).
  // These vnames are assembled into vsignatures, which are used to
  // query the registry corresponding to the algorithm.
  //
  // The compile-time signatures (sname and ssignature) are built
  // explicitly when registering an algorithm.  This happens in
  // vcsn/ctx/instantiate.hh, but also in the generated plugins.  For
  // instance:
  //
  // static bool conjunction_vector ATTRIBUTE_USED =
  //   vcsn::dyn::detail::conjunction_vector_register(
  //     vcsn::ssignature<t0_t, t1_t>(),     // <============ Here.
  //     vcsn::dyn::detail::conjunction_vector<t0_t, t1_t>
  //   );
  //
  // So:
  //
  // - sname (built from types) is used at register time, it should
  //   be the precise type at hand,
  //
  // - vname (built from values) is used at dyn:: call time.  It should
  //   return what sname will return.

  /*---------------.
  | Static names.  |
  `---------------*/

  template <typename T>
  struct snamer
  {
    static symbol name()
    {
      return T::sname();
    }
  };

  template <typename T>
  symbol sname()
  {
    return snamer<T>::name();
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
    static symbol name(T& t)
    {
      return t->vname();
    }
  };

  template <typename T>
  symbol vname(T& t)
  {
    return vnamer<T>::name(t);
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
    static symbol name()                        \
    {                                           \
      auto res = symbol{#__VA_ARGS__};          \
      return res;                               \
    }                                           \
  };                                            \
                                                \
  template <>                                   \
  struct vnamer<__VA_ARGS__>                    \
  {                                             \
    static symbol name(__VA_ARGS__&)            \
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
    static symbol name()
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
    static symbol name(type)
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
    static symbol name(integral_constant t)
    {
      return t.name;
    }
  };


  /*--------------.
  | std::tuple.   |
  `--------------*/

  /// The vname of a vector of dyn:: objects (e.g., automaton,
  /// expression, ...) is the *tuple* of their vnames.
  ///
  /// This is used to dispatch variadic calls on vectors of automata
  /// to tuples of vcsn:: automata.
  template <typename Dyn>
  struct dyn_vector_vnamer
  {
    using type = const std::vector<Dyn>;
    static symbol name(const type& t)
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

  /// vector<dyn::automata> -> std::tuple<automaton_t, ...>.
  template <>
  struct vnamer<const std::vector<dyn::automaton>>
    : dyn_vector_vnamer<dyn::automaton>
  {};

  /// vector<dyn::expansion> -> std::tuple<expansion_t, ...>.
  template <>
  struct vnamer<const std::vector<dyn::expansion>>
    : dyn_vector_vnamer<dyn::expansion>
  {};

  /// vector<dyn::expression> -> std::tuple<expression_t, ...>.
  template <>
  struct vnamer<const std::vector<dyn::expression>>
    : dyn_vector_vnamer<dyn::expression>
  {};

  /// vector<dyn::polynomial> -> std::tuple<polynomial_t, ...>.
  template <>
  struct vnamer<const std::vector<dyn::polynomial>>
    : dyn_vector_vnamer<dyn::polynomial>
  {};

  /// The sname of a tuple is the tuple of the snames.
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

    static symbol name()
    {
      return symbol{"std::tuple<" + name<Args...>() + '>'};
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
