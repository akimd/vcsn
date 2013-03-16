#ifndef VCSN_ALGOS_FSM_HH
# define VCSN_ALGOS_FSM_HH

# include <algorithm>
# include <cassert>
# include <iostream>
# include <sstream>
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
  void
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
    out << aut.post() << std::endl;
  }

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      void fsm(const automaton& aut, std::ostream& out)
      {
        fsm(dynamic_cast<const Aut&>(*aut), out);
      }

      using fsm_stream_t =
        auto (const automaton& aut, std::ostream& out) -> void;
      bool fsm_stream_register(const std::string& ctx, const fsm_stream_t& fn);
    }
  }


  /*-----------------.
  | fsm(automaton).  |
  `-----------------*/

  /// The automaton in Att as a string.  Exact type.
  template <class A>
  inline
  std::string
  fsm(const A& aut)
  {
    std::ostringstream o;
    fsm(aut, o);
    return o.str();
  }

  namespace dyn
  {
    namespace details
    {
      /// Abstract but parameterized.
      template <typename Aut>
      std::string fsm(const automaton& aut)
      {
        return fsm(dynamic_cast<const Aut&>(*aut));
      }

      using fsm_string_t = auto (const automaton& aut) -> std::string;

      bool fsm_string_register(const std::string& ctx, const fsm_string_t& fn);
    }
  }
}

#endif // !VCSN_ALGOS_FSM_HH
