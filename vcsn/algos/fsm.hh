#ifndef VCSN_ALGOS_FSM_HH
# define VCSN_ALGOS_FSM_HH

# include <algorithm>
# include <iostream>
# include <unordered_map>
# include <vector>

# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*-------------------------.
  | fsm(automaton, stream).  |
  `-------------------------*/

  template <class A>
  std::ostream&
  fsm(const A& aut, std::ostream& out)
  {
    using label_t = typename A::label_t;

    // This format uses integers for labels.  Reserve 0 for epsilon.
    std::unordered_map<label_t, unsigned> names;
    for (auto t : aut.all_transitions())
      {
        size_t label
          = names.emplace(aut.label_of(t), names.size() + 1).first->second;
        out << aut.src_of(t) << '\t'
            << aut.dst_of(t) << '\t'
            << label << '\t'
            << 1 // FIXME: aut.weight_of(t)
            << std::endl;
      }
    // post is the only final state.
    return out << aut.post() << std::endl;
  }

  namespace dyn
  {
    namespace details
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
