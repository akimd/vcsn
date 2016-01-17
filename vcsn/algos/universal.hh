#pragma once

#include <vcsn/algos/copy.hh> // make_fresh_automaton
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  namespace detail
  {
    /// Functor for universal.
    template <Automaton Aut>
    class universaler
    {
    public:
      static_assert(labelset_t_of<Aut>::is_free(),
                    "universal: requires free labelset");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "universal: requires Boolean weights");

      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using state_set_t = std::set<state_t>;
      using pstate_t = std::set<state_set_t>;
      using map_t = std::map<state_t, state_set_t>;

      /// The universal automaton of \a automaton.
      automaton_t operator()(const Aut& automaton)
      {
        if (!is_deterministic(automaton))
          return work_(determinize(automaton)->strip());
        else if (!is_complete(automaton))
          return work_(complete(automaton));
        else
          return work_(automaton);
      }

    private:
      /// Work on \a aut, which is complete and deterministic.
      /// \pre is_complete(aut)
      /// \pre is_deterministic(aut)
      automaton_t work_(const automaton_t& aut)
      {
        // The initial state of automaton.
        state_t i = aut->dst_of(initial_transitions(aut).front());

        // compute the co-determinized of the minimal automaton
        // and retrieve the origin of each state.
        const auto transposed = transpose(aut);
        auto codet = determinize(transposed);
        map_t origin = codet->origins();
        origin.erase(codet->pre());
        origin.erase(codet->post());

        // the 'origin' is a map from co_det's state_t to
        // minimal's state_set_t.
        // let 'transp_states' be the image of 'origin'.
        pstate_t transp_states = image(origin);

        // the universal automaton's state set is its intersection closure.
        pstate_t univers_states(intersection_closure(transp_states));

        // The universal automaton.
        automaton_t res = make_fresh_automaton(aut);

        // The final states of aut.
        std::set<state_t> automaton_finals;
        for (auto t: final_transitions(aut))
          automaton_finals.insert(aut->src_of(t));

        // we have to save the state set associated to each automaton.
        map_t subset_label;

        // X = univers_states \ {}.
        for (const auto s: univers_states)
          if (!s.empty())
            {
              state_t new_s = res->new_state();
              subset_label[new_s] = s;
              // J = { X | i in X }
              if (has(s, i))
                res->set_initial(new_s);
              // U = { X | X \subset T }
              if (subset(s, automaton_finals))
                res->set_final(new_s);
            }

        // Finally, the transition set.
        for (const auto x: res->states())
          for (const auto y: res->states())
            for (const auto a: *res->labelset())
              {
                bool cont = false;
                state_set_t delta_ret;
                for (auto s: subset_label[x])
                  {
                    bool empty = true;
                    for (auto t: out(aut, s, a))
                      {
                        empty = false;
                        delta_ret.insert(aut->dst_of(t));
                      }
                    if (empty)
                      {
                        cont = true;
                        break;
                      }
                  }
                // case 1: \exists p \in X, p.a = {}
                if (cont)
                  continue;
                // case 2: X.a \subset Y?
                if (subset(delta_ret, subset_label[y]))
                  res->new_transition(x, y, a);
              }
        return res;
      }
    };
  }

  template <Automaton Aut>
  inline
  Aut
  universal(const Aut& a)
  {
    detail::universaler<Aut> universal;
    return universal(a);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      universal(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::universal(a));
      }
    }
  }
}
