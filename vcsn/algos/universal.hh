#ifndef VCSN_ALGOS_UNIVERSAL_HH
# define VCSN_ALGOS_UNIVERSAL_HH

# include <map>

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/set.hh>
# include <vcsn/ctx/lal_char_b.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/transpose.hh>

namespace vcsn
{
  template <typename Aut>
  Aut
  universal(const Aut& automaton)
  {
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean weights");

    using automaton_t = Aut;

    // Start from a complete, deterministic automaton.
    const automaton_t* aut = &automaton;
    std::unique_ptr<automaton_t> complete_deterministic;
    if (!is_deterministic(automaton))
      {
        complete_deterministic.reset(new automaton_t{determinize(automaton)});
        aut = complete_deterministic.get();
      }
    else if (!is_complete(automaton))
      {
        complete_deterministic.reset(new automaton_t{complete(automaton)});
        aut = complete_deterministic.get();
      }

    using state_t = typename automaton_t::state_t;
    using state_set_t = std::set<state_t>;
    using pstate_t = std::set<state_set_t>;
    using map_t = std::map<state_t, state_set_t>;

    // The initial state of automaton.
    state_t i = automaton.dst_of(aut->initial_transitions().front());

    // compute the co-determinized of the minimal automaton
    // and retrieve the origin of each state.
    // FIXME: Get rid of the const_cast.
    // FIXME: Make transposed const.
    auto transposed = transpose(const_cast<Aut&>(*aut));
    detail::determinizer<decltype(transposed)> determinize;
    auto const co_det = determinize(transposed);
    map_t origin = determinize.origins();

    // the 'origin' is a map from co_det's state_t to
    // minimal's state_set_t.
    // let 'transp_states' be the image of 'origin'.
    pstate_t transp_states = image(origin);

    // the universal automaton's state set is its intersection closure.
    pstate_t univers_states(intersection_closure(transp_states));

    // The universal automaton.
    automaton_t res{aut->context()};

    // The final states of aut.
    std::set<state_t> automaton_finals;
    for (auto t: aut->final_transitions())
      automaton_finals.insert(aut->src_of(t));

    // we have to save the state set associated to each automaton.
    map_t subset_label;

    // X = univers_states \ {}.
    for (const auto s: univers_states)
      if (!s.empty())
        {
          state_t new_s = res.new_state();
          subset_label[new_s] = s;
          // J = { X | i in X }
          if (s.find(i) != s.end())
            res.set_initial(new_s);
          // RES = { X | X \subset T }
          if (subset(s, automaton_finals))
            res.set_final(new_s);
        }

    // finally, the transition set.
    for (const auto x: res.states())
      for (const auto y: res.states())
        for (const auto a: *res.labelset())
          {
            bool cont = false;
            state_set_t delta_ret;
            for (auto s: subset_label[x])
              {
                bool empty = true;
                for (auto t: aut->out(s, a))
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
              res.add_transition(x, y, a);
          }
    return res;
  }


  /*-----------------.
  | dyn::universal.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut>
      automaton
      universal(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), universal(a));
      }

      REGISTER_DECLARE(universal,
                       (const automaton& aut) -> automaton);
    }
  }

}

#endif // !VCSN_ALGOS_UNIVERSAL_HH
