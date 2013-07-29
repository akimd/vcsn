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

      vcsn::lazy_automaton_editor edit;
      {
        // Line: Source Destination Label.
        std::string line;
        string_t s1, s2, l;
        do
        {
          std::getline(*fin.get(), line, '\n');
          std::istringstream ss{line};
          ss >> s1 >> s2 >> l;
          if ( ss.fail())
            break;
          if ( s1 == "EOFSM")
            break;
          if (s1 == "0")
            edit.add_initial(s2, l);
          else
            edit.add_entry(s1, s2, l);
        } while (fin.get()->good());

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

