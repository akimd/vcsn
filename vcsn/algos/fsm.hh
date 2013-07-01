#ifndef VCSN_ALGOS_FSM_HH
# define VCSN_ALGOS_FSM_HH

# include <iostream>
# include <unordered_map>

# include <vcsn/core/kind.hh>
# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*-------------------------.
  | fsm(automaton, stream).  |
  `-------------------------*/
  namespace detail
  {
    template <typename LabelSet>
    typename std::enable_if<is_lal<LabelSet>::value, void>::type
    map_one_to_zero(const LabelSet&,
                    std::unordered_map<typename LabelSet::label_t, unsigned>&)
    {}

    template <typename LabelSet>
    typename std::enable_if<!is_lal<LabelSet>::value, void>::type
    map_one_to_zero(const LabelSet& ls,
                    std::unordered_map<typename LabelSet::label_t, unsigned>& map)
    {
      map[ls.one()] = 0;
    }
  }


  // http://www2.research.att.com/~fsmtools/fsm/man4/fsm.5.html
  template <class Aut>
  std::ostream&
  fsm(const Aut& aut, std::ostream& out)
  {
    using label_t = typename Aut::label_t;

    // The FSM format uses integers for labels.  Reserve 0 for epsilon
    // (and the special symbol, that flags initial and final
    // transitions).
    std::unordered_map<label_t, unsigned> names;
    names[aut.labelset()->special()] = 0;
    detail::map_one_to_zero(*aut.labelset(), names);

    // A counter used to name the labels.
    unsigned name = 1;

    for (auto t : aut.all_transitions())
      {
        const auto& lbl = aut.label_of(t);
        auto insert = names.emplace(lbl, name);
        if (insert.second)
          ++name;
        out << aut.src_of(t) << '\t'
            << aut.dst_of(t) << '\t'
            << insert.first->second // << '\t'
          //            << 0 // Output label: epsilon.
          // FIXME: aut.weight_of(t)
            << std::endl;
      }
    // post is the only final state.
    return out << aut.post();
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      std::ostream& fsm(const automaton& aut, std::ostream& out)
      {
        return fsm(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(fsm,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_FSM_HH
