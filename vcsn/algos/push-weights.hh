#pragma once

#include <vcsn/algos/distance.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/unordered_map.hh>

namespace vcsn
{
  /*--------------.
  | push weights. |
  `--------------*/

  /// Find shorhest of \a s0 to the final states of \a aut
  /// by using single source shortest distance.
  template <Automaton Aut>
  weight_t_of<Aut>
  shortest_distance_to_finals(Aut aut, state_t_of<Aut> s0)
  {
    auto d = ss_shortest_distance(aut, s0);
    return d[aut->post()];
  }

  /// Find all shortest distances of each state
  /// to the final states of \a aut.
  template <Automaton Aut>
  std::unordered_map<state_t_of<Aut>, weight_t_of<Aut>>
  shortest_distance_to_finals(Aut aut)
  {
    std::unordered_map<state_t_of<Aut>, weight_t_of<Aut>> res;
    for (auto s : aut->states())
      {
        auto w = shortest_distance_to_finals(aut, s);
        res.emplace(s, w);
      }
    return res;
  }

  /// The algorithm weight pushing.
  /// 1) Calculate shortest distance each state s to final states.
  /// 2) Update the weight of each transition by using shortest distance.
  template <Automaton Aut>
  auto
  push_weights(const Aut& aut)
    -> decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    auto distances = shortest_distance_to_finals(res);
    auto ws = *res->weightset();
    distances[res->post()] = ws.one();
    for (auto t : all_transitions(res))
      {
        auto ds = distances[res->src_of(t)];
        auto de = distances[res->dst_of(t)];
        auto w = ws.mul(res->weight_of(t), de);
        if (res->src_of(t) == res->pre())
          res->set_weight(t, de);
        else if (!ws.is_zero(ds))
          res->set_weight(t, ws.rdivide(w, ds));
      }
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      push_weights(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::push_weights(a);
      }
    }
  }

} // namespace vcsn
