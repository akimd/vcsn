#include <fstream>
#include <set>
#include <string>

#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>

#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/parse.hh>

namespace vcsn
{
  namespace detail
  {
    dyn::automaton
    efsm::parse_file(const std::string& file)
    {
      std::shared_ptr<std::istream> fin;
      if (file == "-")
      {
        struct noop
        {
          void operator()(...) const {}
        };
        fin.reset(&std::cin, noop());
      }
      else
        fin.reset(new std::ifstream(file.c_str()));

      using string_t =
        boost::flyweight<std::string, boost::flyweights::no_tracking>;

      // Search is_one symbol after cat is read.
      {
        std::string cat;
        while (!fin.get()->eof() && cat != "cat")
          *fin >> cat;
        if (fin.get()->eof())
          throw std::runtime_error(file
                                   + ": no \"cat\" symbol");
        fin.get()->ignore(1024, '\n');
      }

      // Search is_one.
      std::string is_one;
      {
        std::string line;
        std::string val;
        while(fin.get()->good())
        {
          std::getline(*fin.get(), line, '\n');
          std::istringstream ss{line};
          // Eat blank lines.
          ss >> is_one;
          if (ss.fail())
            continue;
          ss >> val;
          if (ss.fail())
            throw std::runtime_error (file + ": bad input format");
          if (val == "0" || is_one == "EOFSM")
            break;
        }
      }

      // Parse after cat is read.
      {
        std::string cat;
        while (!fin.get()->eof() && cat != "cat")
          *fin >> cat;
        if (fin.get()->eof())
          throw std::runtime_error(file
                                   + ": no \"cat\" symbol");
        fin.get()->ignore(1024, '\n');
      }

#define SKIP_SPACES()                           \
      while (isspace(fin.get()->peek()))        \
        fin.get()->ignore()
      vcsn::lazy_automaton_editor edit;
      // Get initial state + transition
      {
        // Eat empty lines
        SKIP_SPACES();
        std::string line;
        string_t s1, s2, l;
        std::getline(*fin.get(), line, '\n');
        std::istringstream ss{line};
        ss >> s1 >> s2 >> l;
        edit.add_initial(s1, string_t{});
        if (ss.fail())
            edit.add_final(s1, string_t{});
        else
          edit.add_entry(s1, s2, l);
      }

      {
        // Line: Source Destination Label.
        std::string line;
        string_t s1, s2, l;
        do
        {
          std::getline(*fin.get(), line, '\n');
          std::istringstream ss{line};
          // Eat blank lines.
          SKIP_SPACES();
          ss >> s1 >> s2 >> l;
          if ( s1 == "EOFSM")
            break;
          if (ss.fail())
            edit.add_final(s1, string_t{});
          else
            edit.add_entry(s1, s2, l);
          if (l == is_one)
            l = "\\e";
        } while (fin.get()->good());
#undef SKIP_SPACES

        // Add finals.
        while (s1 != "EOFSM")
        {
          edit.add_final(s1, string_t{});
          *fin.get() >> s1;
        }

      }
      dyn::automaton res = nullptr;
      res.reset(edit.result());
      return res;
    }
  }
}
