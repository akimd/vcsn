#ifndef VCSN_ALGOS_DOT_HH
# define VCSN_ALGOS_DOT_HH

# include <algorithm>
# include <cassert>
# include <iostream>
# include <sstream>
# include <unordered_map>
# include <unordered_set>
# include <vector>

# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*-------------------------.
  | dot(automaton, stream).  |
  `-------------------------*/

  template <class A>
  void
  dot(const A& aut, std::ostream& out)
  {
    using state_t = typename A::state_t;
    using transition_t = typename A::transition_t;
    std::unordered_map<state_t, unsigned> names;

    out <<
      "digraph\n"
      "{\n"
      "  vcsn_context = \"" << aut.context().vname() << "\"\n"
      "  rankdir = LR\n"
      "  node [shape = circle]\n";

    // Name all the states.
    for (auto s : aut.states())
      names[s] = names.size();

    // Output the pre-initial and post-final states.
    out <<
      "  {\n"
      "    node [style = invis, shape = none, label = \"\""
      ", width = 0, height = 0]\n";
    for (auto t : aut.initial_transitions())
      out << "    I" << names[aut.dst_of(t)] << '\n';
    for (auto t : aut.final_transitions())
      out << "    F" << names[aut.src_of(t)] << '\n';
    out << "  }\n";

    // Output all the states to make "print | read" idempotent.
    out << "  {";
    for (auto s : aut.states())
      out << ' ' << names[s];
    out << " }\n";

    for (auto src : aut.all_states())
      {
        // Transitions from src, ordered by destination.
        // Keep a single destination as we use entries below.
        // FIXME: A heap might be nice.
        std::unordered_set<state_t> ds;
        std::vector<transition_t> ts;
        for (auto t: aut.all_out(src))
          if (ds.find(aut.dst_of(t)) == end(ds))
            {
              ds.insert(aut.dst_of(t));
              ts.push_back(t);
            }
        std::sort(begin(ts), end(ts),
                  [&aut](transition_t a, transition_t b)
                  {
                    return aut.dst_of(a) < aut.dst_of(b);
                  });
        for (const auto& t: ts)
          {
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
            std::string s = aut.entryset().format(aut.entry_at(t), ", ");
            if (!s.empty())
              {
                out << " [label = \"";
                str_escape(out, s) << "\"]";
              }
            out << "\n";
          }
      }
    out << "}";
  }

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      void dot(const automaton& aut, std::ostream& out)
      {
        dot(dynamic_cast<const Aut&>(*aut), out);
      }

      using dot_stream_t =
        auto (const automaton& aut, std::ostream& out) -> void;
      bool dot_stream_register(const std::string& ctx, dot_stream_t fn);
    }
  }


  /*-----------------.
  | dot(automaton).  |
  `-----------------*/

  /// The automaton in Dot as a string.  Exact type.
  template <class A>
  inline
  std::string
  dot(const A& aut)
  {
    std::ostringstream o;
    dot(aut, o);
    return o.str();
  }

  namespace dyn
  {
    namespace details
    {
      /// Abstract but parameterized.
      template <typename Aut>
      std::string dot(const automaton& aut)
      {
        return dot(dynamic_cast<const Aut&>(*aut));
      }

      using dot_string_t = auto (const automaton& aut) -> std::string;

      bool dot_string_register(const std::string& ctx, dot_string_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_DOT_HH
