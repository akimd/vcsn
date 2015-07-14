#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/signature-printer.hh>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace ast
  {

    /// We managed to read \a res in \a is, check that \a is is
    /// finished.
    static void check_eof(std::istream& is, std::shared_ptr<ast_node> res)
    {
      if (is.peek() != EOF)
        {
          std::ostringstream o;
          signature_printer printer(o, true);
          res->accept(printer);
          vcsn::fail_reading(is,
                             "unexpected trailing characters after '",
                             o.str(), "'");
        }
    }

    std::shared_ptr<ast_node> context_parser::parse()
    {
      auto res = any_();
      check_eof(is_, res);
      return res;
    }

    std::shared_ptr<ast_node> context_parser::parse_context()
    {
      auto res = context_();
      check_eof(is_, res);
      return res;
    }

    std::shared_ptr<ast_node> context_parser::any_()
    {
      std::string w = word_();
      auto res = std::shared_ptr<ast_node>{};
      if (w == "delay_automaton"
          || w == "determinized_automaton"
          || w == "detweighted_automaton"
          || w == "expression_automaton"
          || w == "filter_automaton"
          || w == "focus_automaton"
          || w == "mutable_automaton"
          || w == "pair_automaton"
          || w == "partition_automaton"
          || w == "permutation_automaton"
          || w == "product_automaton"
          || w == "scc_automaton"
          || w == "synchronized_automaton"
          || w == "transpose_automaton"
          || w == "tuple_automaton")
        res = automaton_(w);
      else if (has(labelsets_, w)
               || w == "expressionset"
               || w == "seriesset")
        {
          res = labelset_(w);
          if (is_.peek() == ',')
            res = context_(res);
        }
      else if (w == "lat")
        {
          res = tupleset_();
          if (is_.peek() == ',')
            res = context_(res);
        }
      else if (has(weightsets_, w))
        res = weightset_(w);
      else if (w == "expansionset")
        res = expansionset_();
      else if (w == "polynomialset")
        res = polynomialset_();
      // std::integral_constant<unsigned, 2>.
      else if (w == "std::integral_constant")
        {
          w += eat(is_, '<');
          w += word_();
          w += eat(is_, ',');
          w += word_();
          w += eat(is_, '>');
          res = std::make_shared<other>(w);
        }
      // boost::optional<unsigned>, std::vector<unsigned>.
      else if (w == "boost::optional"
               || w == "const std::vector")
        {
          w += eat(is_, '<');
          w += word_();
          w += eat(is_, '>');
          res = std::make_shared<other>(w);
        }
      else
        res = std::make_shared<other>(w);
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

    std::string context_parser::word_()
    {
      std::string res;
      int c;
      while (is_.peek() == ' ')
        is_.get();
      while ((c = is_.get()) != EOF)
        if (c == '<' || c == ',' || c == '>' || c == '(')
          {
            is_.unget();
            break;
          }
        else
          res += c;
      return res;
    }

    std::shared_ptr<const genset>
    context_parser::genset_(const std::string& letter_type)
    {
      std::string gens;
      if (is_.peek() == '(')
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
    context_parser::context_(const std::shared_ptr<ast_node>& ls)
    {
      eat(is_, ',');
      while (isspace(is_.peek()))
        is_.ignore();
      return std::make_shared<context>(ls, weightset_());
    }

    std::shared_ptr<context>
    context_parser::context_(const std::string& word)
    {
      return context_(labelset_(word));
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
      else if (ls == "lan" | ls == "nullableset")
        {
          eat(is_, '<');
          auto res = labelset_();
          if (!res->has_one())
            res = std::make_shared<nullableset>(res);
          eat(is_, '>');
          return res;
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
      else if (ls == "letterset")
        {
          eat(is_, '<');
          auto gs = genset_();
          eat(is_, '>');
          return std::make_shared<letterset>(gs);
        }
      else if (ls == "wordset")
        {
          eat(is_, '<');
          auto gs = genset_();
          eat(is_, '>');
          return std::make_shared<wordset>(gs);
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
    context_parser::automaton_(std::string prefix)
    {
      std::shared_ptr<automaton> res = nullptr;
      // focus_automaton<TapeNum, Aut>.
      if (prefix == "focus_automaton")
        {
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            std::make_shared<other>(word_()));
          eat(is_, ',');
          res->get_content().emplace_back(automaton_(word_()));
          eat(is_, '>');
        }
      // xxx_automaton<Aut>.
      else if (prefix == "delay_automaton"
               || prefix == "determinized_automaton"
               || prefix == "detweighted_automaton"
               || prefix == "expression_automaton"
               || prefix == "filter_automaton"
               || prefix == "pair_automaton"
               || prefix == "partition_automaton"
               || prefix == "permutation_automaton"
               || prefix == "scc_automaton"
               || prefix == "synchronized_automaton"
               || prefix == "transpose_automaton")
        {
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            automaton_(word_()));
          eat(is_, '>');
        }
      // mutable_automaton<Context>.
      else if (prefix == "mutable_automaton")
        {
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            context_());
          eat(is_, '>');
        }
      // xxx_automaton<Aut...>.
      else if (prefix == "product_automaton"
               || prefix == "tuple_automaton")
        {
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            automaton_(word_()));
          while (is_.peek() == ',')
            {
              eat(is_, ',');
              res->get_content().emplace_back(automaton_(word_()));
            }
          eat(is_, '>');
        }
      else
        raise("invalid automaton name: ", str_escape(prefix));
      return res;
    }

    std::shared_ptr<tupleset>
    context_parser::tupleset_()
    {
      eat(is_, '<');
      typename tupleset::value_t res;
      res.emplace_back(labelset_or_weightset_());
      while (is_.peek() == ',')
      {
        eat(is_, ',');
        res.emplace_back(labelset_or_weightset_());
      }
      eat(is_, '>');
      return std::make_shared<tupleset>(res);
    }

    std::shared_ptr<expressionset>
    context_parser::expressionset_()
    {
      eat(is_, '<');
      auto context = context_();
      eat(is_, '>');
      auto ids = rat::identities{};
      if (is_.peek() == '(')
        {
          eat(is_, '(');
          is_ >> ids;
          eat(is_, ')');
        }
      return std::make_shared<expressionset>(context, ids);
    }

    std::shared_ptr<expressionset>
    context_parser::seriesset_()
    {
      eat(is_, '<');
      auto context = context_();
      eat(is_, '>');
      return std::make_shared<expressionset>(context,
                                             rat::identities::distributive);
    }

    std::shared_ptr<expansionset> context_parser::expansionset_()
    {
      eat(is_, '<');
      eat(is_, "expressionset");
      auto res = std::make_shared<expansionset>(expressionset_());
      eat(is_, '>');
      return res;
    }

    std::shared_ptr<polynomialset> context_parser::polynomialset_()
    {
      eat(is_, '<');
      auto res = std::make_shared<polynomialset>(context_());
      eat(is_, '>');
      return res;
    }
  }
}
