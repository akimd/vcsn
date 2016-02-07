#pragma once

#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  /*------------------.
  | tags definitions  |
  `------------------*/

  /// Tag to request the most appropriate version of an algorithm.
  struct auto_tag {};

  /// Tag for operations on deterministic automata.
  struct deterministic_tag {};

  /// Tag for operations on all automata.
  struct general_tag {};

  /// Tag for operations on standard automata.
  struct standard_tag {};

  namespace detail
  {
    /// Dispatch an operation between automata depending on their nature.
    /// TO FIX
    template <Automaton... Aut, typename Operation>
    auto
    dispatch_tags_(std::false_type,
                   std::string algo, Operation op, Aut&&... auts)
    {
      if (algo == "auto")
        {
          if (all(is_standard(std::forward<Aut>(auts))...))
            algo = "standard";
          else
            algo = "general";
        }

      if (algo == "general")
        return op(general_tag{});
      else if (algo == "standard")
        return op(standard_tag{});
      else
        raise("invalid algorithm: ", str_escape(algo));
    }

    template <Automaton... Aut, typename Operation>
    auto
    dispatch_tags_(std::true_type,
                   std::string algo, Operation op, Aut&&... auts)
    {
      if (algo == "auto")
        {
          if (all(is_standard(std::forward<Aut>(auts))...))
            algo = "standard";
          else if (all(is_deterministic(auts)...))
            algo = "deterministic";
          else
            algo = "general";
        }

      if (algo == "general")
        return op(general_tag{});
      else if (algo == "standard")
        return op(standard_tag{});
      else if (algo == "deterministic")
        return op(deterministic_tag{});
      else
        raise("invalid algorithm: ", str_escape(algo));
    }

    template <Automaton... Aut, typename Operation>
    auto
    dispatch_tags(std::string algo, Operation op, Aut&&... auts)
    {
      constexpr bool is_free
        = all_<labelset_t_of<decltype(auts)>::is_free()...>();
      return dispatch_tags_(bool_constant<is_free>{},
                            algo,
                            std::forward<Operation>(op),
                            std::forward<Aut>(auts)...);
    }

    /// Make an empty automaton which is a supertype of others.
    template <Automaton... Auts>
    auto
    make_join_automaton(deterministic_tag, Auts&&... auts)
    {
      return join_automata(std::forward<Auts>(auts)...);
    }

    /// Make an empty automaton which is a supertype of others, and
    /// with a nullable labelset.
    template <Automaton... Auts>
    auto
    make_join_automaton(general_tag, Auts&&... auts)
      // SFINAE
      -> decltype(nullable_join_automata(std::forward<Auts>(auts)...))
    {
      return nullable_join_automata(std::forward<Auts>(auts)...);
    }

    /// Make an empty automaton which is a supertype of others.
    template <Automaton... Auts>
    auto
    make_join_automaton(standard_tag, Auts&&... auts)
    {
      return join_automata(std::forward<Auts>(auts)...);
    }
  }

  /// Request the Boolean specialization for determinization (B and
  /// F2).
  struct boolean_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"boolean_tag"};
      return res;
    }
  };

  /// Dijkstra implementation.
  ///
  /// Used for lightest_path and scc.
  struct dijkstra_tag {};

  /// Request for the weighted version of an algorithm.
  ///
  /// Used for determinization, and minimization.
  struct weighted_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"weighted_tag"};
      return res;
    }
  };
}
