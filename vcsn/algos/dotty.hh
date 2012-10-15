#ifndef VCSN_ALGOS_DOTTY_HH
# define VCSN_ALGOS_DOTTY_HH

# include <iostream>
# include <set>
# include <sstream>
# include <unordered_map>

# include <vcsn/core/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*---------------------------.
  | dotty(automaton, stream).  |
  `---------------------------*/

  template <class A>
  void
  dotty(const A& aut, std::ostream& out)
  {
    using state_t = typename A::state_t;
    std::unordered_map<state_t, unsigned> names;

    out <<
      "digraph\n"
      "{\n"
      "  vcsn_context = \"" << aut.context().sname() << "\"\n"
      "  vcsn_genset = \"" << aut.context().genset_string() << "\"\n"
      "  rankdir = LR\n"
      "  node [shape = circle]\n";

    // Name all the states.
    for (auto s : aut.states())
      names[s] = names.size();

    // Output the pre-initial and post-final states.
    out <<
      "  {\n"
      "    node [style = invis, shape = none, label = \"\", width = 0, height = 0]\n";
    for (auto t : aut.initial_transitions())
      out << "    I" << names[aut.dst_of(t)] << '\n';
    // Output the "post" final states.
    for (auto t : aut.final_transitions())
      out << "    F" << names[aut.src_of(t)] << '\n';
    out << "  }\n";

    // Output the states that are not part of an entry.
    {
      std::set<state_t> reachable;
      for (auto t : aut.all_entries())
        {
          reachable.insert(aut.src_of(t));
          reachable.insert(aut.dst_of(t));
        }
      for (auto s : aut.states())
        if (reachable.find(s) == reachable.end())
          out << "  " << names[s] << "\n";
    }

    for (auto t : aut.all_entries())
      {
	auto src = aut.src_of(t);
	auto dst = aut.dst_of(t);

	if (src == aut.pre())
	  {
	    unsigned n = names[dst];
	    out << "  I" << n << " -> " << n;
	  }
	else if (dst == aut.post())
	  {
	    unsigned n = names[src];
	    out << "  " << n << " -> F" << n;
	  }
	else
	  {
	    unsigned ns = names[src];
	    unsigned nd = names[dst];
	    out << "  " << ns << " -> " << nd;
	  }
	std::string s = aut.entryset().format(aut.entry_at(t));
	if (!s.empty())
	  {
	    out << " [label = \"";
	    str_escape(out, s) << "\"]";
	  }
	out << "\n";
      }
    out << "}" << std::endl;
  }

  template <typename Aut>
  void abstract_dotty(const dyn::abstract_automaton& aut,
                      std::ostream& out)
  {
    dotty(dynamic_cast<const Aut&>(aut), out);
  }

  using dotty_stream_t =
    auto (const dyn::abstract_automaton& aut, std::ostream& out) -> void;

  bool dotty_register(const std::string& ctx, const dotty_stream_t& fn);

  void dotty(const dyn::abstract_automaton& aut, std::ostream& out);


  /*-------------------.
  | dotty(automaton).  |
  `-------------------*/

  /// The automaton in Dot as a string.  Exact type.
  template <class A>
  inline
  std::string
  dotty(const A& aut)
  {
    std::ostringstream o;
    dotty(aut, o);
    return o.str();
  }

  /// Abstract but parameterized.
  template <typename Aut>
  std::string abstract_dotty(const dyn::abstract_automaton& aut)
  {
    return dotty(dynamic_cast<const Aut&>(aut));
  }

  using dotty_string_t =
    auto (const dyn::abstract_automaton& aut) -> std::string;

  bool dotty_register(const std::string& ctx, const dotty_string_t& fn);

  /// Abstract.
  std::string dotty(const dyn::abstract_automaton& aut);

}

#endif // !VCSN_ALGOS_DOTTY_HH
