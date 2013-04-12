#ifndef VCSN_ALGOS_INFO_HH
# define VCSN_ALGOS_INFO_HH

# include <iostream>
# include <sstream>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <class A>
  void
  info(const A& aut, std::ostream& out)
  {
    out
      << "context: " << aut.vname(true) << std::endl
      << "number of states: " << aut.num_states() << std::endl
      << "number of initial states: " << aut.num_initials() << std::endl
      << "number of final states: " << aut.num_finals() << std::endl
      << "number of transitions: " << aut.num_transitions() << std::endl
      //<< "is complete: " << is_complete(aut) << std::endl
      << "is deterministic: " << is_deterministic(aut) << std::endl
      << "is eps-acyclic: " << is_eps_acyclic(aut) << std::endl
      << "is proper: " << is_proper(aut) << std::endl
      << "is valid: " << is_valid(aut);
      ;
  }

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      void info(const automaton& aut, std::ostream& out)
      {
        info(dynamic_cast<const Aut&>(*aut), out);
      }

      using info_stream_t =
        auto (const automaton& aut, std::ostream& out) -> void;
      bool info_stream_register(const std::string& ctx, info_stream_t fn);
    }
  }


  /*------------------.
  | info(automaton).  |
  `------------------*/

  /// The automaton in Info as a string.  Exact type.
  template <class A>
  inline
  std::string
  info(const A& aut)
  {
    std::ostringstream o;
    info(aut, o);
    return o.str();
  }

  namespace dyn
  {
    namespace details
    {
      /// Abstract but parameterized.
      template <typename Aut>
      std::string info(const automaton& aut)
      {
        return info(dynamic_cast<const Aut&>(*aut));
      }

      using info_string_t = auto (const automaton& aut) -> std::string;

      bool info_string_register(const std::string& ctx, info_string_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_INFO_HH
