#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*---------------------.
  | has_negative_cycle.  |
  `---------------------*/

  template <typename Aut>
  bool has_negative_cycle(const Aut& aut)
  {
    using weight_t = weight_t_of<Aut>;
    using state_t = state_t_of<Aut>;
    auto size = aut->all_states().back() + 1;
    auto l = std::vector<boost::optional<weight_t>>(size);
    auto ws = *aut->weightset();

    for (auto s: aut->states())
      if (aut->is_initial(s))
        l[s] = ws.one();

    for (auto n: aut->states())
      for (auto t: aut->transitions())
        {
          auto src = aut->src_of(t);
          auto dst = aut->dst_of(t);
          auto w = aut->weight_of(t);
          if (l[src])
            if (!l[dst] || ws.less(ws.mul(*l[src], w), *l[dst]))
              l[dst] = ws.mul(*l[src], w);
        }

    for (auto t: aut->transitions())
      {
        auto src = aut->src_of(t);
        auto dst = aut->dst_of(t);
        auto w = aut->weight_of(t);
        if (l[src])
          if (!l[dst] || ws.less(ws.mul(*l[src], w), *l[dst]))
            return true;
      }

    return false;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <typename Aut>
      bool has_negative_cycle(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_negative_cycle(a);
      }
    }
  }
}
