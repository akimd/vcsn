#pragma once

#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/is-free.hh>
#include <vcsn/algos/minimize-brzozowski.hh>
#include <vcsn/algos/minimize-hopcroft.hh>
#include <vcsn/algos/minimize-moore.hh>
#include <vcsn/algos/minimize-signature.hh>
#include <vcsn/algos/minimize-weighted.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  struct minimize_ftag; // Defined in minimize-brzozowski.hh
  CREATE_FUNCTION_TAG(cominimize);

  /// Minimization via the minimizer functors.
  ///
  /// \tparam Aut  the input automaton type.
  /// \tparam Tag  the requested algorithm:
  ///      moore_tag, signature_tag, weighted_tag.
  template <Automaton Aut, typename Tag>
  auto
  minimize(const Aut& a, Tag)
    -> quotient_t<Aut>
  {
    auto minimize = detail::minimizer<Aut, Tag>{a};
    auto res = quotient(a, minimize.classes());
    res->properties().update(minimize_ftag{});
    return res;
  }

  /// Minimization for Boolean automata: auto_tag.
  template <Automaton Aut>
  std::enable_if_t<std::is_same_v<weightset_t_of<Aut>, b>,
                    quotient_t<Aut>>
  minimize(const Aut& a, auto_tag = {})
  {
    return minimize(a, signature_tag{});
  }

  /// Minimization for non Boolean automata: auto_tag..
  template <Automaton Aut>
  std::enable_if_t<!std::is_same_v<weightset_t_of<Aut>, b>,
                    quotient_t<Aut>>
  minimize(const Aut& a, auto_tag = {})
  {
    return minimize(a, weighted_tag{});
  }

  /// Minimization for Boolean automata: algo selection.
  ///
  /// \param a     the automaton
  /// \param algo  the algorithm to run.
  template <Automaton Aut>
  std::enable_if_t<std::is_same_v<weightset_t_of<Aut>, b>,
                   quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo)
  {
    if (is_free(a))
      return minimize_free_boolean(a, algo);
    else
      return minimize_non_free_boolean(a, algo);
  }

  /// Minimization for free Boolean automata: algo selection.
  ///
  /// \param a     the automaton
  /// \param algo  the algorithm to run.
  template <Automaton Aut>
  auto minimize_free_boolean(const Aut& a, const std::string& algo)
  {
    static const auto map
      = getarg<std::function<quotient_t<Aut>(const Aut&)>>
    {
      "minimization algorithm",
      {
        {"auto",      [](const Aut& a){ return minimize(a, auto_tag{}); }},
        {"hopcroft",  [](const Aut& a){ return minimize(a, hopcroft_tag{}); }},
        {"moore",     [](const Aut& a){ return minimize(a, moore_tag{}); }},
        {"signature", [](const Aut& a){ return minimize(a, signature_tag{}); }},
        {"weighted",  [](const Aut& a){ return minimize(a, weighted_tag{}); }},
      }
    };
    return map[algo](a);
  }


  /// Minimization for non-free Boolean automata: algo selection.
  ///
  /// \param a     the automaton
  /// \param algo  the algorithm to run.
  template <Automaton Aut>
  auto minimize_non_free_boolean(const Aut& a, const std::string& algo)
  {
    static const auto map
      = getarg<std::function<quotient_t<Aut>(const Aut&)>>
    {
      "minimization algorithm",
      {
        {"auto",      [](const Aut& a){ return minimize(a, auto_tag{}); }},
        {"signature", [](const Aut& a){ return minimize(a, signature_tag{}); }},
        {"weighted",  [](const Aut& a){ return minimize(a, weighted_tag{}); }},
      }
    };
    return map[algo](a);
  }

  /// Minimization for non Boolean automata: algo selection.
  ///
  /// \param a     the automaton
  /// \param algo  the algorithm to run.
  template <Automaton Aut>
  std::enable_if_t<!std::is_same_v<weightset_t_of<Aut>, b>,
                    quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo)
  {
    static const auto map
      = getarg<std::function<quotient_t<Aut>(const Aut&)>>
    {
      "minimization algorithm",
      {
        {"auto",     [](const Aut& a){ return minimize(a, auto_tag{}); }},
        {"weighted", [](const Aut& a){ return minimize(a, weighted_tag{}); }},
      }
    };
    return map[algo](a);
  }

  /// Cominimization.
  template <Automaton Aut, typename Tag = auto_tag>
  auto
  cominimize(const Aut& a, Tag tag = {})
    -> decltype(transpose(minimize(transpose(a), tag)))
  {
    auto res = transpose(minimize(transpose(a), tag));
    res->properties().update(cominimize_ftag{});
    return res;
  }

  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif
      /// Helper function to facilitate dispatch below.
      template <Automaton Aut, typename Tag>
      automaton minimize_tag_(const Aut& aut)
      {
        // There are several "minimize" that will match this
        // definition: some are vcsn::minimize (e.g., for
        // weighted_tag), but others will comes from vcsn::dyn::detail
        // (e.g., for brzozowski_tag when inapplicable).  So do not
        // specify the namespace.
        return minimize(aut, Tag{});
      }
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      minimize(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "minimization algorithm",
            {
              {"auto",       minimize_tag_<Aut, auto_tag>},
              {"brzozowski", minimize_tag_<Aut, brzozowski_tag>},
              {"hopcroft",   minimize_tag_<Aut, hopcroft_tag>},
              {"moore",      minimize_tag_<Aut, moore_tag>},
              {"signature",  minimize_tag_<Aut, signature_tag>},
              {"weighted",   minimize_tag_<Aut, weighted_tag>},
            }
          };
        return map[algo](aut->as<Aut>());
      }
    }
  }


  /*-----------------.
  | dyn::cominimize. |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Helper function to facilitate dispatch below.
      template <Automaton Aut, typename Tag>
      automaton cominimize_tag_(const Aut& aut)
      {
        return transpose(minimize(transpose(aut), Tag{}));
      }

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      cominimize(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "cominimization algorithm",
            {
              {"auto",       cominimize_tag_<Aut, auto_tag>},
              {"brzozowski", cominimize_tag_<Aut, brzozowski_tag>},
              {"hopcroft",   cominimize_tag_<Aut, hopcroft_tag>},
              {"moore",      cominimize_tag_<Aut, moore_tag>},
              {"signature",  cominimize_tag_<Aut, signature_tag>},
              {"weighted",   cominimize_tag_<Aut, weighted_tag>},
            }
          };
        return map[algo](aut->as<Aut>());
      }
    }
  }

} // namespace vcsn
