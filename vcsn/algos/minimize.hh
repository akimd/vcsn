#pragma once

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
  namespace detail
  {
    template <typename Aut>
    constexpr bool can_use_brzozowski()
    {
      return labelset_t_of<Aut>::is_free()
        && std::is_same<weightset_t_of<Aut>, b>::value;
    }
  }

  // FIXME: there must exist some nicer way to do this.
  template <typename Aut>
  inline
  vcsn::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value
                    && labelset_t_of<Aut>::is_free(),
                    quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo = "auto")
  {
    if (algo == "hopcroft")
      return minimize(a, hopcroft_tag{});
    else if (algo == "moore")
      return minimize(a, moore_tag{});
    else if (algo == "auto" || algo == "signature")
      return minimize(a, signature_tag{});
    else if (algo == "weighted")
      return minimize(a, weighted_tag{});
    else
      raise("minimize: invalid algorithm (Boolean, free labelset): ",
            str_escape(algo));
  }

  template <typename Aut>
  inline
  vcsn::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value
                    && ! labelset_t_of<Aut>::is_free(),
                    quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo = "auto")
  {
    if (algo == "auto" || algo == "signature")
      return minimize(a, signature_tag{});
    else if (algo == "weighted")
      return minimize(a, weighted_tag{});
    else
      raise("minimize: invalid algorithm (Boolean, non-free labelset): ",
            str_escape(algo));
  }

  template <typename Aut>
  inline
  vcsn::enable_if_t<!std::is_same<weightset_t_of<Aut>, b>::value,
                    quotient_t<Aut>>
  minimize(const Aut& a, const std::string& algo = "auto")
  {
    if (algo == "auto" || algo == "weighted")
      return minimize(a, weighted_tag{});
    else
      raise("minimize: invalid algorithm (non-Boolean): ", str_escape(algo));
  }

  template <typename Aut>
  inline
  auto
  cominimize(const Aut& a, const std::string& algo = "auto")
    -> decltype(transpose(minimize(transpose(a), algo)))
  {
    return transpose(minimize(transpose(a), algo));
  }


  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut, typename String>
      inline
      vcsn::enable_if_t<::vcsn::detail::can_use_brzozowski<Aut>(), automaton>
      minimize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "brzozowski")
          return make_automaton(::vcsn::minimize(a, brzozowski_tag{}));
        else
          return make_automaton(::vcsn::minimize(a, algo));
      }

      template <typename Aut, typename String>
      inline
      vcsn::enable_if_t<!::vcsn::detail::can_use_brzozowski<Aut>(), automaton>
      minimize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::minimize(a, algo));
      }

      /// Bridge.
      template <typename Aut, typename String>
      inline
      automaton
      minimize(const automaton& aut, const std::string& algo)
      {
        return minimize_<Aut, String>(aut, algo);
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
      template <typename Aut, typename String>
      inline
      vcsn::enable_if_t<::vcsn::detail::can_use_brzozowski<Aut>(), automaton>
      cominimize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "brzozowski")
          return make_automaton(::vcsn::cominimize(a, brzozowski_tag{}));
        else
          return make_automaton(::vcsn::cominimize(a, algo));
      }

      template <typename Aut, typename String>
      inline
      vcsn::enable_if_t<!::vcsn::detail::can_use_brzozowski<Aut>(), automaton>
      cominimize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::cominimize(a, algo));
      }

      /// Bridge.
      template <typename Aut, typename String>
      inline
      automaton
      cominimize(const automaton& aut, const std::string& algo)
      {
        return cominimize_<Aut, String>(aut, algo);
      }
    }
  }

} // namespace vcsn
