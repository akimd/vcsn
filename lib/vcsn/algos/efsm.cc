#include <fstream>
#include <set>
#include <string>

#include <vcsn/misc/flyweight.hh>

#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/algos/fwd.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/efsm.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    automaton
    read_efsm(std::istream& is)
    {
      std::string file = "file.efsm";
      using string_t =
        boost::flyweight<std::string, boost::flyweights::no_tracking>;

      // Look for the symbol table.
      {
        std::string cat;
        while (!is.eof() && cat != "cat")
          is >> cat;
        if (is.eof())
          throw std::runtime_error(file
                                   + ": no \"cat\" symbol");
        is.ignore(1024, '\n');
      }

      // The single piece of information we need from the symbol
      // table: the representation of the empty word.
      std::string one;
      {
        std::string line;
        std::string val;
        while (is.good())
          {
            std::getline(is, line, '\n');
            std::istringstream ss{line};
            // Eat blank lines.
            ss >> one;
            if (ss.fail())
              continue;
            ss >> val;
            if (ss.fail())
              throw std::runtime_error (file + ": bad input format");
            if (val == "0" || one == "EOFSM")
              break;
          }
      }

      // Parse after cat is read.
      {
        std::string cat;
        while (!is.eof() && cat != "cat")
          is >> cat;
        if (is.eof())
          throw std::runtime_error(file
                                   + ": no \"cat\" symbol");
        is.ignore(1024, '\n');
      }

#define SKIP_SPACES()                           \
      while (isspace(is.peek()))        \
        is.ignore()
      vcsn::lazy_automaton_editor edit;
      // Get initial state + transition
      {
        // Eat empty lines
        SKIP_SPACES();
        std::string line;
        string_t s, d, l, w;
        std::getline(is, line, '\n');
        std::istringstream ss{line};
        ss >> s >> d >> l >> w;
        edit.add_initial(s);
        if (l == one)
          l = "\\e";
        if (l.get().empty())
          // FinalState [Weight]
          edit.add_final(s, d);
        else
          // Src Dst Lbl Wgt
          edit.add_transition(s, d, l, w);
      }

      {
        // Line: Source Destination Label [Weight].
        std::string line;
        do
          {
            std::getline(is, line, '\n');
            if (line == "EOFSM")
              break;
            std::istringstream ss{line};
            // Eat blank lines.
            SKIP_SPACES();
            string_t s, d, l, w;
            ss >> s >> d >> l >> w;
            if (l == one)
              l = "\\e";
            if (l.get().empty())
              // FinalState [Weight]
              edit.add_final(s, d);
            else
              // Src Dst Lbl Wgt
              edit.add_transition(s, d, l, w);
          } while (is.good());
#undef SKIP_SPACES

        if (line != "EOFSM")
          throw std::runtime_error(file + ":bad input format, missing EOFSM");
      }
      return edit.result();
    }


    /*------------.
    | efsm(aut).  |
    `------------*/

    REGISTER_DEFINE2(efsm);

    std::ostream&
    efsm(const automaton& aut, std::ostream& out)
    {
      detail::efsm_registry().call(aut, out);
      return out;
    }
  }
}
