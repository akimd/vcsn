#include <fstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <lib/vcsn/algos/fwd.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/read.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/regex.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace
    {
      using string_t = symbol;

      /// Try to read the context in first line.
      std::string read_context(std::string& line)
      {
        // A context definition possibly followed by comments.
        static auto re = std::regex("^[ \t]*(?:vcsn_)?(?:context|ctx)[ \t]*="
                                    "[ \t]*(\"?)(.*?)\\1[ \t]*(?://.*)?$");
        // To handle escaped characters.
        static auto sub = std::regex("\\\\\\\\");
        std::smatch m;
        if (std::regex_match(line, m, re))
        {
          std::string res = m[2];
          if (m[1] == "\"")
            res = std::regex_replace(res, sub, "\\");
          return res;
        }
        else
          return "";
      }

      /// Get the escaped string between quotes.
      std::string read_quotes(std::istream& is)
      {
        std::string res;
        int c;
        while ((c = is.peek()) != EOF && c != '"')
          res += get_char(is);
        if (c != '"')
          raise("invalid daut file: missing '\"' after '" + res + "'");
        is.ignore(); // Eat the final '"'
        return res;
      }

      /// Check if there is a comment. In case of "//", put the first '/' back
      /// in the stream.
      bool is_comment(std::istream& is, char c)
      {
        if (c == '/' && is.peek() == '/')
          {
            is.unget(); // Put back the first '/'
            return true;
          }
        else
          return c == '#';
      }


      /// Read a state or possibly an arrow ("->")
      std::string read_state(std::istream& is)
      {
        std::string res;
        int c;
        skip_space(is);
        while ((c = is.get()) != EOF)
          {
            if (c == '\\')
            {
              if ((c = is.get()) != EOF)
                res += c;
              continue;
            }
            if (c == '"')
              return read_quotes(is);
            if (isspace(c) || is_comment(is, c))
              break;
            res += c;
          }
        boost::algorithm::trim_right(res);
        return res;

      }

      /// Read string until the end or a comment.
      std::string read_entry(std::istream& is)
      {
        std::string res;
        int c;
        skip_space(is);
        while ((c = is.get()) != EOF)
          {
            if (isspace(c) && res.empty())
              continue;
            if (is_comment(is, c))
              break;
            res += c;
          }
        return res;
      }

      vcsn::automaton_editor* make_editor(std::string ctx)
      {
        auto c = vcsn::dyn::make_context(ctx.empty() ? "lal_char, b" : ctx);
        auto res = vcsn::dyn::make_automaton_editor(c);
        res->set_separator(',');
        res->add_pre(string_t{"$pre"});
        res->add_post(string_t{"$post"});
        return res;
      }
    }

    automaton
    read_daut(std::istream& is, const location&)
    {
      auto first = true;
      std::shared_ptr<vcsn::automaton_editor> edit = nullptr;

      // Line: Source [->] Dest [Entry]
      std::string line;
      while (is.good())
        {
          std::getline(is, line, '\n');
          // Trim here to handle line full of blanks.
          boost::algorithm::trim_right(line);
          if (line.empty() || boost::starts_with(line, "//"))
            continue;

          if (first)
            {
              auto ctx = read_context(line);
              edit.reset(make_editor(ctx));
              first = false;
              if (!ctx.empty())
                continue;
            }

          std::istringstream ss{line};
          auto s = string_t{read_state(ss)};
          auto d = string_t{read_state(ss)};
          if (d.get().empty()) // Declaring a state with no transitions
            {
              edit->add_state(s);
              continue;
            }
          if (d == "->")
            d = read_state(ss);
          require(!d.get().empty(),
                  "invalid daut file: expected destination after: ", s);
          auto entry = string_t{read_entry(ss)};
          try
            {
              edit->add_entry(s == "$" ? string_t{"$pre"} : s,
                              d == "$" ? string_t{"$post"} : d, entry);
            }
          catch (const std::runtime_error& e)
            {
              raise(e, "  while adding transitions: (", s, ", ", entry, ", ",
                    d, ')');
            }
        }

      if (!edit)
        edit.reset(make_editor(""));
      return edit->result();
    }
  }
}
