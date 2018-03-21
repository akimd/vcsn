#include <fstream>
#include <string>

#include <vcsn/config.hh>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp> // starts_with
#include <boost/algorithm/string/replace.hpp> // replace_all_copy
#include <boost/iostreams/filter/bzip2.hpp>
#if VCSN_HAVE_BOOST_IOSTREAMS_FILTER_LZMA_HPP
# include <boost/iostreams/filter/lzma.hpp>
#endif
#include <boost/iostreams/filtering_stream.hpp>

#include <lib/vcsn/algos/fwd.hh>
#include <vcsn/algos/edit-automaton.hh>
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
      template <typename... Args>
      ATTRIBUTE_NORETURN
      void
      raise_invalid(const location& loc, Args&&... args)
      {
        const auto& file
          = loc.begin.filename ? *loc.begin.filename : "file.efsm";
        raise(file, ": invalid efsm file: ",
              std::forward<Args>(args)...);
      }

      /// Look for the next "cat >$medir/FILE <<\EOFSM" file,
      /// and return FILE.
      std::string
      read_here_doc(std::istream& is, const location& loc)
      {
        static const auto re
          = std::regex("cat >\\$medir/([a-z]+)\\.[a-z]* <<\\\\EOFSM",
                       std::regex::extended);
        std::string line;
        std::smatch res;
        while (is.good())
          {
            std::getline(is, line, '\n');
            if (std::regex_match(line, res, re))
              return res[1];
          }
        raise_invalid(loc, "missing \"cat\" symbol");
      }

      /// Read a symbol table (i.e., eat up to the next EOFSM) and
      /// return the single piece of information we need from the
      /// symbol table: the representation of the empty word.
      std::string
      read_symbol_table(std::istream& is, const location& loc)
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
              raise_invalid(loc);
            if (val == "0" || res == "EOFSM")
              break;
          }

        while (line != "EOFSM" && is.good())
            std::getline(is, line, '\n');

        if (line != "EOFSM")
          raise_invalid(loc, "missing closing EOFSM");
        return res;
      }

      /// Look for the "arc_type=" line that specifies the weightset.
      lazy_automaton_editor::weightset_type
      read_weightset_type(std::istream& is, const location& loc)
      {
        using weightset_type = lazy_automaton_editor::weightset_type;
        std::string line;
        while (is.good())
          {
            std::getline(is, line, '\n');
            if (boost::starts_with(line, "arc_type="))
              {
                boost::algorithm::erase_first(line, "arc_type=");
                static auto map = getarg<weightset_type>
                  {
                    "arc type",
                    {
                      {"log",      weightset_type::logarithmic},
                      {"log64",    weightset_type::logarithmic},
                      {"standard", weightset_type::tropical},
                    }
                  };
                return map[line];
              }
          }
        raise_invalid(loc, "missing \"arc_type=\"");
      }
    }

    automaton
    read_efsm(std::istream& is, const location& loc)
    {
      using string_t = symbol;

      // Whether has both isysmbols and osymbols.
      bool is_transducer = false;

      // Look for the arc type, which describes the weightset.
      auto weightset = read_weightset_type(is, loc);

      // Look for the symbol table.
      auto isyms = read_here_doc(is, loc);
      // The single piece of information we need from the symbol
      // table: the representation of the empty word.
      std::string ione = read_symbol_table(is, loc);

      // If we had "isymbols", we now expect "osymbols".
      std::string oone = ione;
      if (isyms == "isymbols")
      {
        is_transducer = true;
        auto osyms = read_here_doc(is, loc);
        if (osyms != "osymbols")
          raise_invalid(loc, "expected osymbols: ", osyms);
        oone = read_symbol_table(is, loc);
      }

      auto edit = vcsn::lazy_automaton_editor{};
      edit.open(true);
      edit.weightset(weightset);

      // The first transition also provides the initial state.
      bool first = true;
      auto trans = read_here_doc(is, loc);
      if (trans != "transitions")
        raise_invalid(loc, "expected transitions: ", trans);
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
                  edit.add_transition(s, d, l1, l2, w);
                }
              else
                {
                  // l2 is actually the weight.
                  edit.add_transition(s, d, l1, l2);
                }
            }
          first = false;
        }

      if (line != "EOFSM")
        raise_invalid(loc, "missing EOFSM");
      // Flush till EOF.
      while (is.get() != EOF)
        continue;

      // We don't want to read it as a `law<char>` automaton, as for
      // OpenFST, these "words" are insecable.  The proper
      // interpretation is lal<string> (or lan<string>).
      using boost::algorithm::replace_all_copy;
      auto ctx = replace_all_copy(edit.result_context(),
                                  "law<char>", "lan<string>");
      return edit.result(ctx);
    }

    automaton
    read_efsm_bzip2(std::istream& is, const location& loc)
    {
      namespace io = boost::iostreams;
      auto&& in =io::filtering_stream<io::input>{};
      in.push(io::bzip2_decompressor());
      in.push(is);
      return read_efsm(in, loc);
    }

    automaton
    read_efsm_lzma(std::istream& is, const location& loc)
    {
#if VCSN_HAVE_BOOST_IOSTREAMS_FILTER_LZMA_HPP
      namespace io = boost::iostreams;
      auto&& in =io::filtering_stream<io::input>{};
      in.push(io::lzma_decompressor());
      in.push(is);
      return read_efsm(in, loc);
#else
      raise("Boost is too old (", BOOST_LIB_VERSION, ") to support lzma");
#endif
    }
  }
}
