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
  struct general_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"general_tag"};
      return res;
    }
  };

  /// Tag for operations on standard automata.
  struct standard_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"standard_tag"};
      return res;
    }
  };

  namespace detail
  {
    /// Dispatch an operation between automata depending on their nature.
    template <Automaton... Aut, typename Operation>
    auto
    dispatch_tags(std::string algo, Operation op, Aut&&... auts)
    {
      // Whether all the labelsets are letterized.  Used to require is_free
      // to call determinize/is_deterministic.
      // FIXME: Nullableset removal: is_deterministic now accepts non-free
      // automaton. It could be wrong to use deterministic_tag this way.
      // At least, keep check on is_letterized for the moment.
      constexpr bool are_letterized
        = all_<labelset_t_of<decltype(auts)>::is_letterized()...>();

      if (algo == "auto")
        {
          // Applies to both GCC and Clang.  Cannot be done inside a
          // statement.
#if defined __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
          if (all(is_standard(std::forward<Aut>(auts))...))
            algo = "standard";
          else if (static_if<are_letterized>
                   ([](auto&&... as){ return all(is_deterministic(as)...); },
#if (defined __clang__ && __clang_major__ == 3 && __clang_minor__ < 6   \
     || defined __GNUC__ && !defined __clang__ && __GNUC__ < 5)
                     // Clang 3.5 and GCC 4.9 require that we name the argument.
                     [](auto&&... as){ return false; }
#else
                     [](auto&&...){ return false; }
#endif
                     )
                   (std::forward<Aut>(auts)...))
            algo = "deterministic";
          else
            algo = "general";
#if defined __GNUC__
# pragma GCC diagnostic pop
#endif
        }

      if (algo == "general")
        return op(general_tag{});
      else if (algo == "standard")
        return op(standard_tag{});
      else
        {
          using res_t = decltype(op(standard_tag{}));
          res_t res = nullptr;
          if (algo == "deterministic")
            static_if<are_letterized>([&res](auto&& op)
                                { res = op(deterministic_tag{}); })(op);
          require(res, "invalid algorithm: ", str_escape(algo));
          return res;
        }
    }

    /// Make an empty automaton which is a supertype of others.
    template <Automaton... Auts>
    auto
    make_join_automaton(deterministic_tag, Auts&&... auts)
      // SFINAE
      -> decltype(join_automata(std::forward<Auts>(auts)...))
    {
      return join_automata(std::forward<Auts>(auts)...);
    }

    /// Make an empty automaton which is a supertype of others.
    // FIXME: Nullableset removal: possible factorization with
    // deterministic and standard tags version?
    template <Automaton... Auts>
    auto
    make_join_automaton(general_tag, Auts&&... auts)
      // SFINAE
      -> decltype(join_automata(std::forward<Auts>(auts)...))
    {
      return join_automata(std::forward<Auts>(auts)...);
    }

    /// Make an empty automaton which is a supertype of others.
    template <Automaton... Auts>
    auto
    make_join_automaton(standard_tag, Auts&&... auts)
      // SFINAE
      -> decltype(join_automata(std::forward<Auts>(auts)...))
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
