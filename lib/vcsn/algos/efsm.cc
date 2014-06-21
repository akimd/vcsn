#include <fstream>
#include <set>
#include <string>

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/efsm.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/flyweight.hh>
#include <vcsn/misc/regex.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace
    {
      /// Look for the next "cat >$medir/FILE <<\EOFSM" file,
      /// and return FILE.
      std::string
      next_here_doc(std::istream& is)
      {
        static std::regex re("cat >\\$medir/([a-z]+)\\.[a-z]* <<\\\\EOFSM",
                             std::regex::extended);
        std::string line;
        std::smatch res;
        while (is.good())
          {
            std::getline(is, line, '\n');
            if (std::regex_match(line, res, re))
              return res[1];
          }
        raise("invalid file: missing \"cat\" symbol");
      }

      /// Swallow a symbol table (i.e., eat up to the next EOFSM) and
      /// return the single piece of information we need from the
      /// symbol table: the representation of the empty word.
      std::string
      swallow_symbol_table(std::istream& is)
      {
        std::string res;
        std::string line;
        std::string val;
        while (is.good())
          {
            std::getline(is, line, '\n');
            std::istringstream ss{line};
            ss >> res;
            if (ss.fail())
              continue;
            ss >> val;
            if (ss.fail())
              raise("invalid file");
            if (val == "0" || res == "EOFSM")
              break;
          }

        while (line != "EOFSM" && is.good())
            std::getline(is, line, '\n');

        require(line == "EOFSM",
                "invalid file: missing closing EOFSM");
        return res;
      }
    }


    automaton
    read_efsm(std::istream& is)
    {
      std::string file = "file.efsm";
      using string_t =
        boost::flyweight<std::string, boost::flyweights::no_tracking>;

      // Whether has both isysmbols and osymbols.
      bool is_transducer = false;

      // Look for the symbol table.
      auto isyms = next_here_doc(is);
      // The single piece of information we need from the symbol
      // table: the representation of the empty word.
      std::string ione = swallow_symbol_table(is);

      // If we had "isymbols", we now expect "osymbols".
      std::string oone = ione;
      if (isyms == "isymbols")
      {
        is_transducer = true;
        auto osyms = next_here_doc(is);
        require(osyms == "osymbols",
                "invalid file: expected osymbols: ", osyms);
        oone = swallow_symbol_table(is);
      }

      vcsn::lazy_automaton_editor edit;
      edit.open(true);

      // The first transition also provides the initial state.
      bool first = true;
      auto trans = next_here_doc(is);
      require(trans == "transitions",
              "invalid file: expected transitions: ", trans);
      // Line: Source Dest ILabel [OLabel] [Weight].
      // Line: FinalState [Weight].
      std::string line;
      while (is.good())
        {
          std::getline(is, line, '\n');
          if (line == "EOFSM")
            break;
          std::istringstream ss{line};
          string_t s, d, l1, l2, w;
          ss >> s >> d >> l1 >> l2 >> w;
          if (first)
            edit.add_initial(s);
          if (l1.get().empty())
            // FinalState [Weight]
            edit.add_final(s, d);
          else
            {
              if (l1 == ione)
                l1 = "\\e";
              if (is_transducer)
                {
                  if (l2 == oone)
                    l2 = "\\e";
                  l1 = "(" + l1.get() + "," + l2.get() + ")";
                }
              else
                w = l2;
              edit.add_transition(s, d, l1, w);
            }
          first = false;
        }
#undef SKIP_SPACES

      require(line == "EOFSM",
              file, ": bad input format, missing EOFSM");
      // Flush till EOF.
      while (is.get() != EOF)
        continue;
      return edit.result();
    }


    /*------------.
    | efsm(aut).  |
    `------------*/

    REGISTER_DEFINE(efsm);

    std::ostream&
    efsm(const automaton& aut, std::ostream& out)
    {
      detail::efsm_registry().call(aut, out);
      return out;
    }
  }
}
