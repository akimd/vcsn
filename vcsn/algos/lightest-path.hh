#pragma once

#include <vcsn/misc/getargs.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/a-star.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/word-polynomialset.hh>

namespace vcsn
{
  /// Lightest path dispatch between algorithms with tags.
  template <Automaton Aut, typename Tag = dijkstra_tag>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, Tag tag = {})
  {
    return lightest_path(aut, aut->pre(), aut->post(), tag);
  }

  /// Lightest path dispatch between algorithms with string.
  template <Automaton Aut>
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

  template <Automaton Aut>
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
