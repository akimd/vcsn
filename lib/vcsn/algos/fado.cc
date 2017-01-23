#include <fstream>
#include <set>
#include <string>

#include <lib/vcsn/algos/fwd.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {

    /*-----------------.
    | read_fado(aut).  |
    `-----------------*/
    namespace
    {
      /// Read an id until a space or a comment.
      std::string read_id(std::istream& is)
      {
        std::string res;
        int c;
        skip_space(is);
        while ((c = is.get()) != EOF && !isspace(c))
          {
            if (c == '#')
              {
                is.unget();
                break;
              }
            res += c;
          }
        return res;
      }
    }

    automaton
    read_fado(std::istream& is, const location&)
    {
      std::string file = "file.fado";
      using string_t = symbol;

      // The header (the first line) looks like:
      //
      // @NFA 3 4 5 * 0 1 2
      //
      // where 0 1 2 are initial states, and 3 4 5 are final states.
      //
      // The star is optional, in which case the initial state
      // is the src state of the first transition.

      auto edit = vcsn::lazy_automaton_editor{};
      edit.open(true);

      // Whether we process initial states / first line.
      bool init = false;
      bool first = true;
      bool is_transducer = false;
      bool is_dfa = false;
      std::string line, state;
      while (is.good())
        {
          std::getline(is, line, '\n');
          std::istringstream ss{line};
          state = read_id(ss);
          if (state.empty()) // Empty line or comments
            continue;

          if (first)
            {
              is_transducer = state == "@Transducer";
              is_dfa = state == "@DFA";
              if (!is_transducer && !is_dfa && state != "@NFA")
                raise(file, ": bad automaton kind in first line: ", state);

              while (!(state = read_id(ss)).empty())
                {
                  if (state == "$") // Alphabet declaration
                    break;
                  else if (state == "*")
                    {
                      require(!is_dfa, "fado: invalid '*' for DFA"
                              " in first line");
                      require(!init, "fado: multiple '*' in first line");
                      init = true;
                    }
                  else if (init)
                    edit.add_initial(string_t{state});
                  else
                    edit.add_final(string_t{state});
                }
              first = false;
            }
          else
            {
              // Line: Source InputLabel [OutputLabel] Destination.
              auto s1 = string_t{state};
              auto l1 = string_t{read_id(ss)};
              auto l2 = string_t{read_id(ss)};
              auto s2 = string_t{read_id(ss)};
              // First state is our initial state if not declared before by "*".
              if (!init)
                {
                  edit.add_initial(s1);
                  init = true;
                }
              if (l1 == "@epsilon")
                {
                  require(!is_dfa, "fado: unexpected '@epsilon' in DFA, in: ",
                          s1, ' ', l1, ' ',
                          l2, s2.get().empty() ? "" : " ",
                          s2);
                  l1 = "\\e";
                }
              if (is_transducer)
                {
                  if (l2 == "@epsilon")
                    l2 = "\\e";
                  edit.add_transition(s1, s2, l1, l2, string_t{});
                }
              else
                {
                  require(s2 == "",
                          "fado: unexpected trailing characters after: ", s1,
                          ' ', l1, ' ', l2);
                  edit.add_transition(s1, l2, l1);
                }
            }
        }
      return edit.result();
    }
  }// dyn::
}// vcsn::
