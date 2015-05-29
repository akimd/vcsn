#pragma once

#include <vcsn/algos/distance.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/weight.hh>

namespace vcsn
{
  template <typename Aut>
  inline
  auto
  weight_series(const Aut& a)
    -> weight_t_of<Aut>
  {
    state_distancer<Aut, weightset_t_of<Aut>> d(a);
    return d(a->pre(), a->post());
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      weight
      weight_series(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto res = ::vcsn::weight_series(a);
        const auto& ctx = a->context();
        return make_weight(*ctx.weightset(), res);
      }
    }
  }
} // namespace vcsn
