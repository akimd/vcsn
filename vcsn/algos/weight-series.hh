#pragma once

#include <vcsn/algos/evaluate.hh>
#include <vcsn/algos/lightest-path.hh>
#include <vcsn/algos/to-spontaneous.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/weightset/fwd.hh> // nmin

namespace vcsn
{
  template <Automaton Aut>
  auto
  weight_series(const Aut& a)
    -> std::enable_if_t<!is_tropical<weightset_t_of<Aut>>::value,
                        weight_t_of<Aut>>
  {
    try
      {
        auto aut = proper(to_spontaneous(a));
        return evaluate(aut);
      }
    catch (const std::runtime_error& e)
      {
        raise(e, "  while computing weight series");
      }
  }

  template <Automaton Aut>
  auto
  weight_series(const Aut& a)
    -> std::enable_if_t<is_tropical<weightset_t_of<Aut>>::value,
                        weight_t_of<Aut>>
  {
    try
      {
        auto res = path_monomial(a, lightest_path(a));
        return res ? res->second : a->weightset()->zero();
      }
    catch (const std::runtime_error& e)
      {
        raise(e, "  while computing weight series");
      }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      weight
      weight_series(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto res = ::vcsn::weight_series(a);
        const auto& ctx = a->context();
        return {*ctx.weightset(), res};
      }
    }
  }
} // namespace vcsn
