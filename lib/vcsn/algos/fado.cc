#include <fstream>
#include <set>
#include <string>

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/grail.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {

    /*-----------------.
    | read_fado(aut).  |
    `-----------------*/

    automaton
    read_fado(std::istream& is)
    {
      std::string file = "file.fado";
      using string_t = symbol;

      // The header (the first line) looks like:
      //
      // @DFA 3 4 5 * 0 1 2
      //
      // where 0 1 2 are initial states, and 3 4 5 are final states.
      //
      // The star is optional, in which case the initial state
      // is the src state of the first transition.
      {
        std::string kind;
        is >> kind;
        if (kind != "@DFA" && kind != "@NFA")
          raise(file, ": bad automaton kind: ", kind);
      }

      vcsn::lazy_automaton_editor edit;
      edit.open(true);

      char c;
      // Whether we process initial states.
      bool init = false;
      std::string state;
      while ((c = is.get()) != '\n' && !is.eof())
        if (c == ' ' || c == '\t')
          {
            // Eat blanks.
            if (state.empty())
              continue;
            string_t s1{state};
            if (init)
              edit.add_initial(s1);
            else
              edit.add_final(s1);
            state = "";
          }
        else if (c == '*')
          init = true;
        else
          // Grow the name of the state.
          state.append(1, c);

      // Make sure we don't skip last state, which happens
      // when there are no spaces before '\n'.
      if (!state.empty())
        {
          string_t s1{state};
          if (init)
            edit.add_initial(s1);
          else
            edit.add_final(s1);
        }

      {
        // Line: Source Label Destination.
        string_t s1, l, s2;
        is >> s1 >> l >> s2;

        // First state is our initial state if not declared before by "*".
        if (!init && !is.eof())
          edit.add_initial(s1);

        while (!is.eof())
          {
            if (l == "@epsilon")
              l = "\\e";
            edit.add_transition(s1, s2, l);
            is >> s1 >> l >> s2;
          }
      }
      return edit.result();
    }

    /*------------.
    | fado(aut).  |
    `------------*/

    REGISTER_DEFINE(fado);

    std::ostream&
    fado(const automaton& aut, std::ostream& out)
    {
      detail::fado_registry().call(aut, out);
      return out;
    }

  }// dyn::
}// vcsn::
