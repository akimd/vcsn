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

    parse_fado::parse_fado()
      : edit_{nullptr}
    {}

    auto
    parse_fado::parse_file(const std::string& file) -> automaton_t
    {
      context_ = find_ctx(file);
      auto ctx = vcsn::dyn::make_context(context_);
      edit_ = vcsn::dyn::make_automaton_editor(ctx);
      return make_automata(file);
    }

    std::string
    parse_fado::find_ctx(const std::string& file)
    {
      // The alphabet is discovered on the run.
      std::set<char> letters;
      bool is_lan = false;
      std::shared_ptr<std::istream> fin;
      std::string str;
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

      if (!fin.get()->good())
        throw std::runtime_error("unable to read file: " + file);

      while (std::getline (*fin.get(), str).good())
      {
        std::istringstream is(str);
        // Get rid of comments at end of line.
        std::getline(is, str, '#');
        std::istringstream iss(str);
        std::string label;
        iss >> label;
        // Ignore empty lines and comments.
        if (iss.fail() || label[0] == '#')
          continue;
        if (label[0] != '@')
        {
          iss >> label;
          if (iss.fail())
            throw std::runtime_error("bad input file: " + file);
          if (label == "@epsilon")
            is_lan = true;
          for (auto c: label)
            letters.emplace(c);
        }
        ss_ << str << '\n';
      }

      // Result: the context string.
      std::string res = is_lan ? "lan_char(" : "lal_char(";
      for (auto c: letters)
        res += c;
      res += ")_b";
      return res;
    }

    parse_fado::automaton_t
    parse_fado::make_automata(const std::string& file)
    {
      using string_t =
        boost::flyweight<std::string, boost::flyweights::no_tracking>;

      string_t pre{"I"};
      string_t post{"F"};

      edit_->add_pre(pre);
      edit_->add_post(post);

      // put @DFA or @NFA in label
      {
        std::string kind;
        ss_ >> kind;
        if (kind != "@DFA" && kind != "@NFA")
          throw std::runtime_error(file
                                   + ": bad automaton kind: " + kind);
      }

      char c;
      bool init = false;
      std::string state;
      while ((c = ss_.get()) != '\n' && !ss_.eof())
      {
        if (c == ' ' || c == '\t')
        {
          if (state.empty())
            continue;
          string_t s1{state};
          edit_->add_state(s1);
          if (init)
            edit_->add_entry(pre, s1, string_t{});
          else
            edit_->add_entry(s1, post, string_t{});
          state = "";
        }
        else if (c == '*')
          init = true;
        else
          state.append(1, c);
      }

      if (!state.empty())
      {
        string_t s1{state};
        edit_->add_state(s1);
          if (init)
            edit_->add_entry(pre, s1, string_t{});
          else
            edit_->add_entry(s1, post, string_t{});
      }

      {
        // Line: Source Label Destination.
        string_t s1, l, s2;
        ss_ >> s1 >> l >> s2;

        // First state is our pre state if not declared before by "*".
        if (!init && !ss_.eof())
          {
            edit_->add_state(s1);
            edit_->add_entry(pre, s1, string_t{});
          }

        while (!ss_.eof())
          {
            edit_->add_state(s1);
            edit_->add_state(s2);
            edit_->add_entry(s1, s2, l);
            ss_ >> s1 >> l >> s2;
          }
      }
      automaton_t res = nullptr;
      res.reset(edit_->result());
      return res;
    }
  }
}
