#pragma once

#include <stack>

#include <vcsn/algos/accessible.hh> // vcsn::trim
#include <vcsn/algos/is-ambiguous.hh> // is_cycle_ambiguous
#include <vcsn/algos/conjunction.hh> // conjunction
#include <vcsn/algos/scc.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/unordered_map.hh> // vcsn::has

namespace vcsn
{

  /*-----------------.
  | cycle_identity.  |
  `-----------------*/

  namespace detail
  {
    /// Whether all the paths between any two states have the same
    /// weight (i.e., for all s0, s1, any two paths p0, p1 between s0
    /// and s1 have the same weight w_{s0,s1}).
    ///
    /// Because we are on an SCC, it suffices to check on a DFS that
    /// every state is reached with a unique weight from any chosen
    /// "initial" state.
    template <Automaton Aut>
    class cycle_identity_impl
    {
    public:
      using transition_t = transition_t_of<Aut>;
      using weight_t = weight_t_of<Aut>;
      using state_t = state_t_of<Aut>;
      using component_t = detail::component_t<Aut> ;

      /// By DFS starting in s0, check that all the states are reached
      /// with a single weight.
      bool check(const component_t& component, const Aut& aut)
      {
        // FIXME: check ordered_map, or even polynomial of state.
        std::unordered_map<state_t, weight_t> wm;
        // Double-tape weightset.
        const auto& ws = *aut->weightset();
        // Single-tape weightset.
        auto ws1 = ws.template set<0>();
        auto s0 = *component.begin();
        std::stack<state_t> todo;
        todo.push(s0);
        wm[s0] = ws.one();
        while (!todo.empty())
          {
            auto s = todo.top();
            todo.pop();

            for (auto t : all_out(aut, s))
              {
                auto dst = aut->dst_of(t);
                if (has(component, dst))
                  {
                    auto w = ws.mul(wm[s], aut->weight_of(t));
                    if (!has(wm, dst))
                      {
                        todo.push(dst);
                        wm.emplace(dst, w);
                      }
                    else
                      {
                        auto w2 = ws.mul(wm[dst], w);
                        // FIXME: return the counter example?
                        if (!ws1.equal(std::get<0>(w2), std::get<1>(w2)))
                          return false;
                      }
                  }
              }
          }
        return true;
      }
    };
  }

  /// Check the weight of two states on this component is unique.
  template <Automaton Aut>
  bool cycle_identity(const detail::component_t<Aut>& c,
                      const Aut& aut)
  {
    detail::cycle_identity_impl<Aut> ci;
    return ci.check(c, aut);
  }


  /*---------------------.
  | has_twins_property.  |
  `---------------------*/

  /// Create states and the transitions two new automata \a naut1 and
  /// \a naut2 with weight of transition `<(w, one)>` and `<(one, w)>`
  /// corresponding with the weight of transition `<w>` of \a aut.
  template <Automaton AutIn, Automaton AutOut>
  void create_states_and_trans_(const AutIn& aut,
                                AutOut& naut1, AutOut& naut2)
  {
    using state_t = state_t_of<AutIn>;
    std::unordered_map<state_t, state_t_of<AutOut>> ms;

    ms[aut->pre()] = naut1->pre();
    ms[aut->post()] = naut1->post();
    for (auto s : aut->states())
      {
        ms[s] = naut1->new_state();
        naut2->new_state();
      }

    const auto& ws = *aut->weightset();
    for (auto t : aut->all_transitions())
      {
        auto src = aut->src_of(t);
        auto dst = aut->dst_of(t);
        auto w = aut->weight_of(t);
        auto l = aut->label_of(t);
        auto nw1 = std::make_tuple(w, ws.one());
        auto nw2 = std::make_tuple(ws.one(), w);
        naut1->new_transition(ms[src], ms[dst], l, nw1);
        naut2->new_transition(ms[src], ms[dst], l, nw2);
      }
  }

  /// Whether \a aut has the twins property.
  template <Automaton Aut>
  bool has_twins_property(const Aut& aut)
  {
    require(!is_cycle_ambiguous(aut),
            "has_twins_property: requires a cycle-unambiguous automaton");

    // Create new weightset lat<ws, ws> from weightset ws of aut.
    auto ws = *aut->weightset();
    auto nt = std::make_tuple(ws, ws);
    tupleset<decltype(ws), decltype(ws)> nws(nt);

    auto nctx = make_context(*aut->labelset(), nws);
    auto naut1 = make_mutable_automaton(nctx);
    auto naut2 = make_mutable_automaton(nctx);

    auto trim = ::vcsn::trim(aut);
    create_states_and_trans_(trim, naut1, naut2);

    auto a = conjunction(naut1, naut2);

    // Find all components of automate a.
    auto cs = strong_components(a, scc_algo_t::tarjan_iterative);

    // Check unique weight of two states on each component.
    for (const auto& c : cs)
      if (!cycle_identity(c, a))
        return false;

    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool has_twins_property(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_twins_property(a);
      }
    }
  }
}
