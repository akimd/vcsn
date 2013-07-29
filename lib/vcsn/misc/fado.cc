#include <fstream>
#include <set>
#include <string>

#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>

#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/fado.hh>

namespace vcsn
{
  namespace detail
  {
    dyn::automaton
    fado::parse_file(const std::string& file)
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

      // put @DFA or @NFA in kind
      {
        std::string kind;
        *fin >> kind;
        if (kind != "@DFA" && kind != "@NFA")
          throw std::runtime_error(file
                                   + ": bad automaton kind: " + kind);
      }

      char c;
      bool init = false;
      std::string state;
      vcsn::lazy_automaton_editor edit_;
      while ((c = fin.get()->get()) != '\n' && !fin.get()->eof())
      {
        if (c == ' ' || c == '\t')
        {
          // Eat blanks.
          if (state.empty())
            continue;
          string_t s1{state};
          if (init)
            edit_.add_initial(s1, string_t{});
          else
            edit_.add_final(s1, string_t{});
          state = "";
        }
        else if (c == '*')
          init = true;
        else
          state.append(1, c);
      }

      // Make sure we don't skip last state, which happens
      // when there are no spaces before '\n'.
      if (!state.empty())
      {
        string_t s1{state};
          if (init)
            edit_.add_initial(s1, string_t{});
          else
            edit_.add_final(s1, string_t{});
      }

      {
        // Line: Source Label Destination.
        string_t s1, l, s2;
        *fin.get() >> s1 >> l >> s2;

        // First state is our initial state if not declared before by "*".
        if (!init && !fin.get()->eof())
          edit_.add_initial(s1, string_t{});

        while (!fin.get()->eof())
          {
            if (l == "@epsilon")
              l = "\\e";
            edit_.add_entry(s1, s2, l);
            *fin.get() >> s1 >> l >> s2;
          }
      }
      dyn::automaton res = nullptr;
      res.reset(edit_.result());
      return res;
    }
  }
}
