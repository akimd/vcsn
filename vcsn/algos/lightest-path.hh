#pragma once

#include <vcsn/algos/k-lightest-path.hh>
#include <vcsn/algos/a-star.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  /// Lightest path dispatch between algorithms with tags.
  template <Automaton Aut, typename Tag = auto_tag>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, Tag tag = {})
  {
    return lightest_path(aut, aut->pre(), aut->post(), tag);
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                auto_tag = {})
  {
    if (weightset_t_of<Aut>::has_lightening_weights())
      return lightest_path(aut, source, dest, bellman_ford_tag{});
    else
      return lightest_path(aut, source, dest, dijkstra_tag{});
  }

  namespace detail
  {
    /// Tag-based dispatch on implementation.
    template <Automaton Aut, typename Tag>
    std::vector<transition_t_of<Aut>>
    lightest_path_tag(const Aut& aut,
                      state_t_of<Aut> src, state_t_of<Aut> dst)
    {
      return lightest_path(aut, src, dst, Tag{});
    }
  }

  /// Lightest path dispatch between algorithms with string.
  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst,
                const std::string& algo)
  {
    using state_t = state_t_of<Aut>;
    using path_t = std::vector<transition_t_of<Aut>>;
    static const auto map
      = getarg<std::function<path_t(const Aut&, state_t, state_t)>>
    {
      "lightest-path algorithm",
      {
        {"auto",         detail::lightest_path_tag<Aut, auto_tag>},
        {"a-star",       detail::lightest_path_tag<Aut, a_star_tag>},
        {"bellman-ford", detail::lightest_path_tag<Aut, bellman_ford_tag>},
        {"dijkstra",     detail::lightest_path_tag<Aut, dijkstra_tag>},
        {"yen",          detail::lightest_path_tag<Aut, yen_tag>},
      }
    };
    return map[algo](aut, src, dst);
  }

  /// Given a path (typically computed by lightest_path), the
  /// corresponding monomial (label, weight).
  ///
  /// \returns boost::none if there is no path from stc to dst.
  template <Automaton Aut>
  auto
  path_monomial(const Aut& aut,
                const std::vector<transition_t_of<Aut>>& path,
                state_t_of<Aut> src = Aut::element_type::pre(),
                state_t_of<Aut> dst = Aut::element_type::post())
    -> boost::optional<typename detail::word_polynomialset_t<context_t_of<Aut>>::monomial_t>
  {
    auto ps = make_word_polynomialset(aut->context());
    const auto& pls = *ps.labelset();
    const auto& pws = *ps.weightset();
    const auto& ls = *aut->labelset();
    auto w = pws.one();
    auto l = pls.one();
    for (auto t = path[dst]; t != aut->null_transition();
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
