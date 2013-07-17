#ifndef VCSN_ALGOS_DOT_HH
# define VCSN_ALGOS_DOT_HH

# include <algorithm>
# include <iostream>
# include <sstream>
# include <unordered_map>
# include <vector>
# include <set>

# include <vcsn/weights/entryset.hh>

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
    using entryset_t = entryset<context_t>;

    // The main advantage of using entries instead of directly
    // iterating over aut.outin(s, d) is to get a result which is
    // sorted (hence more deterministic).
    auto entry = get_entry(aut, s, d);
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
    // Dot, by default, uses the X11 color naming scheme, whoe "gray"
    // is really light (it almost looks blue in some cases).
    const char* gray = "color = DimGray";

    // Name all the states.
    std::unordered_map<state_t, unsigned> names;
    {
      size_t num = 0;
      for (auto s : aut.states())
        names[s] = num++;
    }

    auto useful = useful_states(aut);

    out <<
      "digraph\n"
      "{\n"
      "  vcsn_context = \"" << aut.context().vname() << "\"\n"
      "  rankdir = LR\n";

    // Output the pre-initial and post-final states.
    if (!aut.initial_transitions().empty()
        || !aut.final_transitions().empty())
      {
        out <<
          "  {\n"
          "    node [style = invis, shape = none, label = \"\""
          ", width = 0, height = 0]\n";
        {
          // Sort by initial states.
          std::set<state_t> ss;
          for (auto t: aut.initial_transitions())
            ss.insert(aut.dst_of(t));
          for (auto s : ss)
            out << "    I" << names[s] << std::endl;
        }
        {
          // Sort by final states.
          std::set<state_t> ss;
          for (auto t: aut.final_transitions())
            ss.insert(aut.src_of(t));
          for (auto s : ss)
            out << "    F" << names[s] << std::endl;
        }
        out << "  }\n";
      }

    // Output all the states to make "print | read" idempotent.
    //
    // Put the useless ones in gray.  This does not work:
    //
    // { 0 1 2 }
    // { node [color = gray] 2 }
    //
    // because 2 was already "declared", and dot does not associate
    // "color = gray" to it.
    if (!aut.states().empty())
      {
        out << "  {" << std::endl
            << "    node [shape = circle]" << std::endl;
        for (auto s : aut.states())
          {
            out << "    " << names[s];
            if (!has(useful, s))
              out << " [" << gray << "]";
            out << std::endl;
          }
        out << "  }" << std::endl;
      }

    for (auto src : aut.all_states())
      {
        // Sort by destination state.
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
            bool useless = !has(useful, src) || !has(useful, dst);
            if (!s.empty() || useless)
              {
                out << " [";
                const char* sep = "";
                if (!s.empty())
                  {
                    out << "label = \"";
                    str_escape(out, s) << "\"";
                    sep = ", ";
                  }
                if (useless)
                  out << sep << gray;
                out << "]";
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
