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

  namespace detail
  {
    /// Tag dispatch on the implementation of weight pushing.
    struct push_weights_distance {};

    /// Find shorhest of \a s0 to the final states of \a aut
    /// by using single source shortest distance.
    template <typename Aut>
    weight_t_of<Aut>
    shortest_distance_to_finals(Aut aut, state_t_of<Aut> s0)
    {
      auto d = ss_shortest_distance(aut, s0);
      return d[aut->post()];
    }

    /// Find all shortest distances of each state
    /// to the final states of \a aut.
    template <typename Aut>
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

    /// In place weight pushing, based on distance.
    ///
    /// 1) Calculate shortest distance each state s to final states.
    /// 2) Update the weight of each transition by using shortest distance.
    template <typename Aut>
    Aut&
    push_weights_here(Aut& res, push_weights_distance)
    {
      auto distances = shortest_distance_to_finals(res);
      auto ws = *res->weightset();
      distances[res->post()] = ws.one();
      for (auto t : res->all_transitions())
        {
          auto ds = distances[res->src_of(t)];
          auto de = distances[res->dst_of(t)];
          if (res->src_of(t) == res->pre())
            res->set_weight(t, de);
          else if (!ws.is_zero(ds))
            res->set_weight(t,
                            ws.rdiv(ws.mul(res->weight_of(t), de), ds));
        }
      return res;
    }

    /// In place weight pushing.
    ///
    /// \param aut   the automaton in which performing the weight pushing.
    /// \param algo  the algorithm to do so.
    template <typename Aut>
    Aut&
    push_weights_here(Aut& res, const std::string& algo = "auto")
    {
      require(algo == "auto" || algo == "distance",
              "push_weights: invalid algorithm: ", algo);
      return push_weights_here(res, push_weights_distance{});
    }
  }

  /// Weight pushing.
  ///
  /// \param aut   the automaton in which performing the weight pushing.
  /// \param algo  the algorithm to do so.
  template <typename Aut>
  auto
  push_weights(const Aut& aut, const std::string& algo = "auto")
    -> decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    detail::push_weights_here(res, algo);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      automaton
      push_weights(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::push_weights(a, algo));
      }
    }
  }
} // namespace vcsn
