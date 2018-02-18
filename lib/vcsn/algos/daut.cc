#include <fstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/rat/caret.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/location.hh>
#include <vcsn/misc/regex.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>

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
      string_t read_quotes(std::istream& is)
      {
        auto res = std::string{};
        int c;
        while ((c = is.peek()) != EOF && c != '"')
          res += get_char(is);
        if (c != '"')
          raise("invalid daut file: missing '\"' after '", res, "'");
        is.ignore(); // Eat the final '"'
        return string_t{res};
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

      /// Read a state ([a-zA-Z0-9_.$-]+) or an arrow ("->")
      string_t read_state(std::istream& is)
      {
        auto res = std::string{};
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
            else if (c == '"')
              return read_quotes(is);
            // `->` is a keyword: "a->b" stands for "a -> b".
            else if (c == '-' && is.peek() == '>')
              {
                if (res.empty())
                  {
                    is.ignore();
                    res = "->";
                  }
                else
                  is.unget();
                break;
              }
            else if (std::isalnum(c)
                     || c == '_' || c == '-' || c == '.' || c == '$')
              res += c;
            else
              {
                is.unget();
                break;
              }
          }
        boost::algorithm::trim_right(res);
        return string_t{res};
      }

      /// Read string until the end or a comment.
      string_t read_entry(std::istream& is)
      {
        auto res = std::string{};
        int c;
        skip_space(is);
        while ((c = is.get()) != EOF)
          {
            if (isspace(c) && res.empty())
              continue;
            else if (is_comment(is, c))
              break;
            else
              res += c;
          }
        return string_t{res};
      }

      const static string_t pre = string_t{"$pre"};
      const static string_t post = string_t{"$post"};

      vcsn::automaton_editor* make_editor(std::string ctx)
      {
        auto c = vcsn::dyn::make_context(ctx.empty() ? "lal, b" : ctx);
        auto res = vcsn::dyn::make_automaton_editor(c);
        res->set_separator(',');
        res->add_pre(pre);
        res->add_post(post);
        return res;
      }
    }

    automaton
    read_daut(std::istream& is, const location& l)
    {
      // FIXME: Making `loc` const was not a good idea.
      location loc = l;
      auto first = true;
      auto edit = std::shared_ptr<vcsn::automaton_editor>{};

      // Line: Source [->] Dest [Entry]
      auto line = std::string{};
      while (is.good())
        {
          loc.step();
          loc.lines(1);
          std::getline(is, line, '\n');
          auto locline = location{loc.begin, loc.begin + line.size()};
          // Trim here to handle line full of blanks.
          boost::algorithm::trim_right(line);
          if (line.empty() || boost::starts_with(line, "//"))
            continue;

          if (first)
            {
              auto ctx = read_context(line);
              try
                {
                  edit.reset(make_editor(ctx));
                }
              catch (const std::runtime_error& e)
                {
                  raise(locline, ": ", e, vcsn::detail::caret(is, locline));
                }
              first = false;
              if (!ctx.empty())
                continue;
            }

          std::istringstream ss{line};
          auto s = read_state(ss);
          auto d = read_state(ss);
          if (d.get().empty()) // Declaring a state with no transitions
            {
              edit->add_state(s);
              continue;
            }
          if (d == "->")
            d = read_state(ss);
          VCSN_REQUIRE(!d.get().empty(),
                       locline, ": ",
                       "invalid daut file: expected destination after: ", s,
                       vcsn::detail::caret(is, locline));
          auto entry = read_entry(ss);
          try
            {
              edit->add_entry(s == "$" ? pre  : s,
                              d == "$" ? post : d, entry);
            }
          catch (const std::runtime_error& e)
            {
              raise(locline, ": ", e, vcsn::detail::caret(is, locline));
            }
        }

      if (!edit)
        edit.reset(make_editor(""));
      return edit->result();
    }
  }
}
