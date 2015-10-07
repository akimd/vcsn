#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/signature-printer.hh>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace ast
  {
    void context_parser::skip_space_()
    {
      while (isspace(is_.peek()))
        is_.ignore();
    }

    int context_parser::peek_()
    {
      skip_space_();
      return is_.peek();
    }

    char context_parser::eat_(char c)
    {
      skip_space_();
      return eat(is_, c);
    }

    const std::string& context_parser::eat_(const std::string& s)
    {
      skip_space_();
      return eat(is_, s);
    }

    void context_parser::check_eof_(std::shared_ptr<ast_node> res)
    {
      if (peek_() != EOF)
        {
          std::ostringstream o;
          signature_printer printer(o, true);
          res->accept(printer);
          vcsn::fail_reading(is_,
                             "unexpected trailing characters after '",
                             o.str(), "'");
        }
    }

    std::string context_parser::word_()
    {
      skip_space_();
      std::string res;
      int c;
      while ((c = is_.peek()) != EOF)
        if (c == '<' || c == ',' || c == '>' || c == '(')
          break;
        else
          {
            res += c;
            is_.ignore();
          }
      // Keep inner spaces, but not trailing spaces.
      boost::algorithm::trim_right(res);
      return res;
    }

    std::string context_parser::parameters_()
    {
      std::string res;
      res += eat_('<');
      auto nesting = 1;
      int c;
      while ((c = peek_()) != EOF)
        {
          if (c == '<')
            ++nesting;
          else if (c == '>' && --nesting == 0)
            break;
          res += c;
          is_.ignore();
        }
      res += eat_('>');
      return res;
    }

    std::shared_ptr<ast_node> context_parser::parse()
    {
      auto res = any_();
      check_eof_(res);
      return res;
    }

    std::shared_ptr<ast_node> context_parser::parse_context()
    {
      auto res = context_();
      check_eof_(res);
      return res;
    }

    std::shared_ptr<ast_node> context_parser::any_()
    {
      std::string w = word_();
      auto res = std::shared_ptr<ast_node>{};
      if (boost::ends_with(w, "_automaton"))
        res = automaton_(w);
      else if (w == "context")
        res = context_(w);
      else if (w == "expansionset")
        res = expansionset_();
      else if (w == "expressionset")
        res = expressionset_();
      else if (has(labelsets_, w))
        res = labelset_(w);
      else if (w == "lat")
        res = tupleset_();
      else if (w == "polynomialset")
        res = polynomialset_();
      else if (w == "seriesset")
        res = seriesset_();
      else if (w == "std::tuple")
        res = tuple_();
      else if (has(weightsets_, w))
        res = weightset_(w);
      else
        // int,
        // std::integral_constant<unsigned, 2>,
        // boost::optional<unsigned>,
        // std::vector<unsigned>,
        // const std::set<std::pair<std::string, std::string>>,
        // etc.
        {
          if (peek_() == '<')
            w += parameters_();
          res = std::make_shared<other>(w);
        }
      return res;
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_or_weightset_()
    {
      return labelset_or_weightset_(word_());
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_or_weightset_(const std::string& w)
    {
      if (w == "lat")
        return tupleset_();
      else if (w == "expressionset")
        return expressionset_();
      else if (w == "seriesset")
        return seriesset_();
      else if (has(labelsets_, w))
        return labelset_(w);
      else if (has(weightsets_, w))
        return weightset_(w);
      else
        raise("invalid weightset or labelset name: " + w);
    }

    std::shared_ptr<const genset>
    context_parser::genset_(std::string letter_type)
    {
      if (letter_type == "char" || letter_type == "string")
        letter_type += "_letters";
      std::string gens;
      if (peek_() == '(')
        {
          gens += '(';
          int c = is_.get();
          while ((c = is_.get()) != EOF && c != ')')
            {
              gens += c;
              if (c == '\\')
                {
                  c = is_.get();
                  require(c != EOF, "unexpected end of file");
                  gens += c;
                }
            }
          gens += ')';
        }
      return std::make_shared<const genset>(letter_type, gens);
    }

    std::shared_ptr<const genset> context_parser::genset_()
    {
      return genset_(word_());
    }

    std::shared_ptr<context> context_parser::context_()
    {
      return context_(word_());
    }

    std::shared_ptr<context>
    context_parser::context_(std::string w)
    {
      bool close = false;
      if (w == "context")
        {
          eat_('<');
          close = true;
          w = word_();
        }
      auto ls = labelset_(w);
      eat_(',');
      auto ws = weightset_();
      if (close)
        eat_('>');
      return std::make_shared<context>(ls, ws);
    }

    std::shared_ptr<ast_node> context_parser::labelset_()
    {
      return labelset_(word_());
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_(const std::string& ls)
    {
      if (ls == "lal_char")
        return std::make_shared<letterset>(genset_("char_letters"));
      else if (ls == "lan")
        {
          // lan<GENSET> => nullableset<letterset<GENSET>>.
          eat_('<');
          auto gs = genset_();
          eat_('>');
          return
            std::make_shared<nullableset>(std::make_shared<letterset>(gs));
        }
      else if (ls == "lan_char")
        return std::make_shared<nullableset>(std::make_shared<letterset>
                                             (genset_("char_letters")));
      else if (ls == "lao")
        return std::make_shared<oneset>();
      else if (ls == "lat")
        return tupleset_();
      else if (ls == "law_char")
        return std::make_shared<wordset>(genset_("char_letters"));
      else if (ls == "lal" || ls == "letterset")
        {
          eat_('<');
          auto gs = genset_();
          eat_('>');
          return std::make_shared<letterset>(gs);
        }
      else if (ls == "law" || ls == "wordset")
        {
          eat_('<');
          auto gs = genset_();
          eat_('>');
          return std::make_shared<wordset>(gs);
        }
      else if (ls == "nullableset")
        {
          eat_('<');
          auto res = labelset_();
          eat_('>');
          if (!res->has_one())
            res = std::make_shared<nullableset>(res);
          return res;
        }
      else if (ls == "expressionset")
        return expressionset_();
      else if (ls == "seriesset")
        return seriesset_();
      raise("invalid labelset name: ", str_escape(ls));
    }

    std::shared_ptr<ast_node> context_parser::weightset_()
    {
      return weightset_(word_());
    }

    std::shared_ptr<ast_node>
    context_parser::weightset_(const std::string& ws)
    {
      if (has(weightsets_, ws))
        return std::make_shared<weightset>(ws);
      else if (ws == "expressionset")
        return expressionset_();
      else if (ws == "seriesset")
        return seriesset_();
      else if (ws == "polynomialset")
        return polynomialset_();
      else if (ws == "lat")
        return tupleset_();
      else
        raise("invalid weightset name: ", str_escape(ws));
    }

    std::shared_ptr<automaton>
    context_parser::automaton_()
    {
      return automaton_(word_());
    }

    std::shared_ptr<automaton>
    context_parser::automaton_(std::string prefix)
    {
      std::shared_ptr<automaton> res = nullptr;
      // focus_automaton<TapeNum, Aut>.
      if (prefix == "focus_automaton")
        {
          eat_('<');
          res = std::make_shared<automaton>(prefix,
                                            std::make_shared<other>(word_()));
          eat_(',');
          res->get_content().emplace_back(automaton_());
          eat_('>');
        }
      // xxx_automaton<Aut>.
      else if (prefix == "delay_automaton"
               || prefix == "expression_automaton"
               || prefix == "filter_automaton"
               || prefix == "insplit_automaton"
               || prefix == "lazy_proper_automaton"
               || prefix == "name_automaton"
               || prefix == "pair_automaton"
               || prefix == "partition_automaton"
               || prefix == "permutation_automaton"
               || prefix == "scc_automaton"
               || prefix == "synchronized_automaton"
               || prefix == "transpose_automaton")
        {
          eat_('<');
          res = std::make_shared<automaton>(prefix, automaton_());
          eat_('>');
        }
      // xxx_automaton<Aut, Tag, Lazy>.
      else if (prefix == "determinized_automaton")
        {
          eat_('<');
          res = std::make_shared<automaton>(prefix, automaton_());
          eat_(',');
          res->get_content().emplace_back(any_());
          eat_(',');
          res->get_content().emplace_back(any_());
          eat_('>');
        }
      // mutable_automaton<Context>.
      else if (prefix == "mutable_automaton")
        {
          eat_('<');
          res = std::make_shared<automaton>(prefix, context_());
          eat_('>');
        }
      // xxx_automaton<ExpresionSet>.
      else if (prefix == "derived_term_automaton")
        {
          eat_("<expressionset");
          res = std::make_shared<automaton>(prefix, expressionset_());
          eat_('>');
        }
      // xxx_automaton<Aut...>.
      else if (prefix == "compose_automaton"
               || prefix == "product_automaton"
               || prefix == "tuple_automaton")
        {
          eat_('<');
          res = std::make_shared<automaton>(prefix, automaton_());
          while (peek_() == ',')
            {
              eat_(',');
              res->get_content().emplace_back(automaton_());
            }
          eat_('>');
        }
      else
        raise("invalid automaton name: ", str_escape(prefix));
      return res;
    }

    std::shared_ptr<tuple>
    context_parser::tuple_()
    {
      eat_('<');
      typename tuple::value_t res;
      res.emplace_back(any_());
      while (peek_() == ',')
      {
        eat_(',');
        res.emplace_back(any_());
      }
      eat_('>');
      return std::make_shared<tuple>(res);
    }

    std::shared_ptr<tupleset>
    context_parser::tupleset_()
    {
      eat_('<');
      typename tupleset::value_t res;
      res.emplace_back(labelset_or_weightset_());
      while (peek_() == ',')
      {
        eat_(',');
        res.emplace_back(labelset_or_weightset_());
      }
      eat_('>');
      return std::make_shared<tupleset>(res);
    }

    std::shared_ptr<expressionset>
    context_parser::expressionset_()
    {
      eat_('<');
      auto context = context_();
      eat_('>');
      auto ids = rat::identities{};
      if (peek_() == '(')
        {
          eat_('(');
          is_ >> ids;
          eat_(')');
        }
      return std::make_shared<expressionset>(context, ids);
    }

    std::shared_ptr<expressionset>
    context_parser::seriesset_()
    {
      eat_('<');
      auto context = context_();
      eat_('>');
      return std::make_shared<expressionset>(context,
                                             rat::identities::distributive);
    }

    std::shared_ptr<expansionset> context_parser::expansionset_()
    {
      eat_('<');
      eat_("expressionset");
      auto res = std::make_shared<expansionset>(expressionset_());
      eat_('>');
      return res;
    }

    std::shared_ptr<polynomialset> context_parser::polynomialset_()
    {
      eat_('<');
      auto res = std::make_shared<polynomialset>(context_());
      eat_('>');
      return res;
    }
  }
}
