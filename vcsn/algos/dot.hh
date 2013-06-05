#ifndef VCSN_ALGOS_DOT_HH
# define VCSN_ALGOS_DOT_HH

# include <algorithm>
# include <iostream>
# include <sstream>
# include <unordered_map>
# include <vector>
# include <set>

# include <vcsn/weights/polynomialset.hh>

# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  template <typename Aut>
  std::string
  format_entry(const Aut& aut,
               typename Aut::state_t s, typename Aut::state_t d)
  {
    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using entryset_t = polynomialset<context_t>;
    using entry_t = typename entryset_t::value_t;

    entry_t entry;
    for (auto t : aut.outin(s, d))
      // Bypass set_weight(), because we know that the weight is
      // nonzero, and that there is only one weight per letter.
      entry[aut.word_label_of(t)] = aut.weight_of(t);
    return entryset_t{aut.context()}.format(entry, ", ");
  }

  /*-------------------------.
  | dot(automaton, stream).  |
  `-------------------------*/

  template <class A>
  std::ostream&
  dot(const A& aut, std::ostream& out)
  {
    using state_t = typename A::state_t;

    // Name all the states.
    std::unordered_map<state_t, unsigned> names;
    {
      size_t num = 0;
      for (auto s : aut.states())
        names[s] = num++;
    }

    out <<
      "digraph\n"
      "{\n"
      "  vcsn_context = \"" << aut.context().vname() << "\"\n"
      "  rankdir = LR\n"
      "  node [shape = circle]\n";

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
        std::set<state_t> ds;
        for (auto t: aut.all_out(src))
          ds.insert(aut.dst_of(t));
        for (auto dst: ds)
          {
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
            std::string s = format_entry(aut, src, dst);
            if (!s.empty())
              {
                out << " [label = \"";
                str_escape(out, s) << "\"]";
              }
            out << "\n";
          }
      }
    return out << "}";
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      std::ostream& dot(const automaton& aut, std::ostream& out)
      {
        return dot(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(dot_stream,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
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
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename Aut>
      std::string dot(const automaton& aut)
      {
        return dot(dynamic_cast<const Aut&>(*aut));
      }

      REGISTER_DECLARE(dot_string,
                       (const automaton& aut) -> std::string);
    }
  }
}

#endif // !VCSN_ALGOS_DOT_HH
