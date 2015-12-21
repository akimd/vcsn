#pragma once

#include <vcsn/misc/getargs.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/algos/is-free-boolean.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/minimize-brzozowski.hh>
#include <vcsn/algos/minimize-hopcroft.hh>
#include <vcsn/algos/minimize-moore.hh>
#include <vcsn/algos/minimize-signature.hh>
#include <vcsn/algos/minimize-weighted.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  template <Automaton Aut>
  std::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value,
                    quotient_t<Aut>>
  minimize(const Aut& a, auto_tag = {})
  {
    return minimize(a, signature_tag{});
  }

  template <Automaton Aut>
  std::enable_if_t<!std::is_same<weightset_t_of<Aut>, b>::value,
                    quotient_t<Aut>>
  minimize(const Aut& a, auto_tag = {})
  {
    return minimize(a, weighted_tag{});
  }

  template <Automaton Aut>
  std::enable_if_t<is_free_boolean<Aut>(), quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo)
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

  template <Automaton Aut>
  std::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value
                    && !labelset_t_of<Aut>::is_free(),
                    quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo)
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

  template <Automaton Aut>
  std::enable_if_t<!std::is_same<weightset_t_of<Aut>, b>::value,
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

  template <Automaton Aut, typename Tag = auto_tag>
  auto
  cominimize(const Aut& a, Tag tag = {})
    -> decltype(transpose(minimize(transpose(a), tag)))
  {
    return transpose(minimize(transpose(a), tag));
  }

  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      minimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();

        static const auto map = getarg<std::function<automaton(const Aut&)>>
        {
          "minimization algorithm",
          {
            {"auto",      [](const Aut& a)
             {
               return make_automaton(minimize(a, auto_tag{}));
             }},
            {"brzozowski",[](const Aut& a)
             {
               return make_automaton(minimize(a, brzozowski_tag{}));
             }},
            {"hopcroft",  [](const Aut& a)
             {
               return make_automaton(minimize(a, hopcroft_tag{}));
             }},
            {"moore",     [](const Aut& a)
             {
               return make_automaton(minimize(a, moore_tag{}));
             }},
            {"signature", [](const Aut& a)
             {
               return make_automaton(minimize(a, signature_tag{}));
             }},
            {"weighted",  [](const Aut& a)
             {
               return make_automaton(minimize(a, weighted_tag{}));
             }},
          }
        };
        return map[algo](a);
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
      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      cominimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();

        static const auto map
          = getarg<std::function<automaton(const Aut&)>>
        {
          "cominimization algorithm",
          {
            {"auto",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                        auto_tag{})));
             }},
            {"brzozowski",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                        brzozowski_tag{})));
             }},
            {"hopcroft",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                        hopcroft_tag{})));
           }},
            {"moore",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                      moore_tag{})));
             }},
            {"signature",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                        signature_tag{})));
             }},
            {"weighted",
             [](const Aut& a)
             {
               return make_automaton(transpose(minimize(transpose(a),
                                                        weighted_tag{})));
             }},
          }
        };
        return map[algo](a);
      }
    }
  }

} // namespace vcsn
