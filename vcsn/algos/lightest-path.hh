#pragma once

#include <vcsn/misc/getargs.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/a-star.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/word-polynomialset.hh>

namespace vcsn
{
  /*--------------------------------.
  | Shortest path algorithm tags.   |
  `--------------------------------*/

  /// A-Star implementation (from vcsn/algos/a-star.hh).
  ///
  /// Uses neutral heuristic.
  /// No preconditions.
  struct astar_tag {};

  /// Bellman-Ford implementation (from vcsn/algos/bellman-ford.hh).
  ///
  /// Raise if the automaton contains a negative loop.
  struct bellmanford_tag {};

  /// Dijkstra implementation (from vcsn/algos/dijkstra.hh).
  ///
  /// Uses fibonacci heap.
  /// No preconditions.
  struct dijkstra_tag {};

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                astar_tag)
  {
    return a_star(aut, source, dest);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                bellmanford_tag)
  {
    return bellman_ford(aut, source, dest);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                dijkstra_tag = {})
  {
    return dijkstra(aut, source, dest);
  }

  template <typename Aut, typename Tag = dijkstra_tag>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, Tag tag = {})
  {
    return lightest_path(aut, aut->pre(), aut->post(), tag);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                const std::string& algo)
  {
    using state_t = state_t_of<Aut>;
    using path_t = std::vector<transition_t_of<Aut>>;
    static const auto map
      = std::map<std::string, std::function<path_t(const Aut&, state_t, state_t)>>
    {
      {"auto",         [](const Aut& a, state_t src, state_t dst)
                       {
                         return lightest_path(a, src, dst);
                       }},
      {"a-star",       [](const Aut& a, state_t src, state_t dst)
                       {
                         return lightest_path(a, src, dst, astar_tag{});
                       }},
      {"bellman-ford", [](const Aut& a, state_t src, state_t dst)
                       {
                         return lightest_path(a, src, dst, bellmanford_tag{});
                       }},
      {"dijkstra",     [](const Aut& a, state_t src, state_t dst)
                       {
                         return lightest_path(a, src, dst, dijkstra_tag{});
                       }},
    };
    auto fun = getargs("algorithm", map, algo);
    return fun(aut, source, dest);
  }

  template <typename Aut>
  auto
  path_monomial(const Aut& aut,
                const std::vector<transition_t_of<Aut>>& path,
                state_t_of<Aut> src = Aut::element_type::pre(),
                state_t_of<Aut> dst = Aut::element_type::post())
    -> boost::optional<typename decltype(make_word_polynomialset(aut->context()))::monomial_t>
  {
    auto ps = make_word_polynomialset(aut->context());
    const auto& pls = *ps.labelset();
    const auto& pws = *ps.weightset();
    const auto& ls = *aut->labelset();
    auto w = pws.one();
    auto l = pls.one();
    auto t = path[dst];
    for (; t != aut->null_transition();
           t = path[aut->src_of(t)])
      {
        w = pws.mul(aut->weight_of(t), w);
        auto nl = aut->label_of(t);
        if (!ls.is_special(nl))
          l = pls.mul(nl, l);
        if (aut->src_of(t) == src)
          return {{l, w}};
      }
    return boost::none;
  }
}
