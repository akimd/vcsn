#include <fstream>
#include <set>
#include <string>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp> // starts_with
#include <boost/algorithm/string/replace.hpp> // replace_all_copy

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/regex.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace
    {
      using string_t = lazy_automaton_editor::string_t;

      /// Turn a label into a parsable label: escape special characters.
      //
      // FIXME: Code duplication with str_escape and
      // edit-automaton.cc, and possibly other places.  Besides, using
      // regex is really overkill, and properly way too costly.  We
      // need a fully fleged str_escape that does not necessarily
      // escapes non-printable.
      std::string quote(string_t s)
      {
        if (s == "\\e"
            || s.get().size() == 1 && std::isalnum(s.get()[0]))
          return s;
        else
          {
            // Backslash backslashes and quotes.
            static auto re = std::regex{"['\\\\]"};
            return ("'"
                    + std::regex_replace(s.get(), re, "\\$&")
                    + "'");
          }
      }

      /// Join two labels into a two-tape label.
      string_t tuple(string_t l0, string_t l1)
      {
        return string_t{l1.get().empty()
                        ? quote(l0)
                        : quote(l0) + "|" + quote(l1)};
      }

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
        raise("invalid efsm file: missing \"cat\" symbol");
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
              raise("invalid efsm file");
            if (val == "0" || res == "EOFSM")
              break;
          }

        while (line != "EOFSM" && is.good())
            std::getline(is, line, '\n');

        require(line == "EOFSM",
                "invalid efsm file: missing closing EOFSM");
        return res;
      }
    }


    automaton
    read_efsm(std::istream& is)
    {
      std::string file = "file.efsm";
      using string_t = symbol;

      // Whether has both isysmbols and osymbols.
      bool is_transducer = false;

      // Look for the arc type, which describes the weightset.
      using weightset_type = lazy_automaton_editor::weightset_type;
      weightset_type weightset = [&is]
        {
          std::string line;
          while (is.good())
            {
              std::getline(is, line, '\n');
              if (boost::starts_with(line, "arc_type="))
                {
                  boost::algorithm::erase_first(line, "arc_type=");
                  static auto map = std::map<std::string, weightset_type>
                  {
                    {"log",      weightset_type::logarithmic},
                    {"log64",    weightset_type::logarithmic},
                    {"standard", weightset_type::tropical},
                  };
                  return getargs("arc type", map, line);
                }
            }
          raise("invalid efsm file: missing \"arc_type=\"");
        }();

      // Look for the context definition.
      dyn::context context = [&is]
        {
          std::string line;
          while (is.good())
            {
              std::getline(is, line, '\n');
              if (boost::starts_with(line, "context='"))
                {
                  boost::algorithm::erase_first(line, "context='");
                  boost::algorithm::erase_last(line, "'");
                  return dyn::make_context(line);
                }
            }
          raise("invalid efsm file: missing \"context=\"");
        }();

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
                "invalid efsm file: expected osymbols: ", osyms);
        oone = swallow_symbol_table(is);
      }

      auto edit = dyn::make_automaton_editor(context);
      //      edit.open(true);
      //      edit.weightset(weightset);

      // The first transition also provides the initial state.
      bool first = true;
      auto trans = next_here_doc(is);
      require(trans == "transitions",
              "invalid efsm file: expected transitions: ", trans);
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
            edit->add_initial(s);
          if (l1.get().empty())
            // FinalState [Weight]
            edit->add_final(s, d);
          else
            {
              if (l1 == ione)
                l1 = "\\e";
              if (is_transducer)
                {
                  if (l2 == oone)
                    l2 = "\\e";
                  edit->add_transition(s, d, tuple(l1, l2), w);
                }
              else
                {
                  // l2 is actually the weight.
                  edit->add_transition(s, d, string_t{quote(l1)}, l2);
                }
            }
          first = false;
        }

      require(line == "EOFSM",
              file, ": bad input format, missing EOFSM");
      // Flush till EOF.
      while (is.get() != EOF)
        continue;

      return edit->result();
    }
  }
}
