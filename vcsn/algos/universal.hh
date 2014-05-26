#ifndef VCSN_ALGOS_UNIVERSAL_HH
# define VCSN_ALGOS_UNIVERSAL_HH

# include <map>

# include <vcsn/misc/set.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/transpose.hh>

namespace vcsn
{
  namespace detail
  {
    /// Functor for universal.
    template <typename Aut>
    class universaler
    {
    public:
      static_assert(labelset_t_of<Aut>::is_free(),
                    "requires labels_are_letters");
      static_assert(std::is_same<weight_t_of<Aut>, bool>::value,
                    "requires Boolean weights");

      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using state_set_t = std::set<state_t>;
      using pstate_t = std::set<state_set_t>;
      using map_t = std::map<state_t, state_set_t>;

      /// The universal automaton of \a automaton.
      automaton_t operator()(const Aut& automaton)
      {
        if (!is_deterministic(automaton))
          return work_(determinize(automaton));
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
        state_t i = aut->dst_of(aut->initial_transitions().front());

        // compute the co-determinized of the minimal automaton
        // and retrieve the origin of each state.
        const auto transposed = transpose(aut);
        detail::determinizer<decltype(transposed)> determinize(transposed);
        // FIXME: we don't need the determinized automaton, just the
        // "origins" map.
        /* auto const co_det = */ determinize();
        map_t origin = determinize.origins();

        // the 'origin' is a map from co_det's state_t to
        // minimal's state_set_t.
        // let 'transp_states' be the image of 'origin'.
        pstate_t transp_states = image(origin);

        // the universal automaton's state set is its intersection closure.
        pstate_t univers_states(intersection_closure(transp_states));

        // The universal automaton.
        automaton_t res
          = std::make_shared<typename automaton_t::element_type>(aut->context());

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
                  res->new_transition(x, y, a);
              }
        return res;
      }
    };
  }

  template <class Aut>
  inline
  Aut
  universal(const Aut& a)
  {
    detail::universaler<Aut> universal;
    return universal(a);
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
        const auto& a = aut->as<Aut>();
        return make_automaton(universal(a));
      }

      REGISTER_DECLARE(universal,
                       (const automaton& aut) -> automaton);
    }
  }

}

#endif // !VCSN_ALGOS_UNIVERSAL_HH
