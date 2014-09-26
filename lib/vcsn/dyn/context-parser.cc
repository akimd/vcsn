#include <vcsn/dyn/context-parser.hh>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace ast
  {

    std::shared_ptr<ast_node> context_parser::parse()
    {
      auto res = any_();
      if (is_.peek() != -1)
        vcsn::fail_reading(is_, "unexpected trailing characters");
      return res;
    }

    std::shared_ptr<ast_node> context_parser::parse_context()
    {
      auto res = context_();
      if (is_.peek() != -1)
        vcsn::fail_reading(is_, "unexpected trailing characters");
      return res;
    }

    std::shared_ptr<ast_node> context_parser::any_()
    {
      std::string w = word();
      if (w == "blind"
          || w == "determinized"
          || w == "detweighted"
          || w == "mutable"
          || w == "pair"
          || w == "partition"
          || w == "permutation"
          || w == "product"
          || w == "ratexp"
          || w == "sub"
          || w == "transpose"
          || w == "tuple")
        return automaton_(w);
      else if (w == "lal"
               || w == "lan"
               || w == "lao"
               || w == "law"
               || w == "ratexpset"
               || w == "seriesset")
        {
          std::shared_ptr<ast_node> res = labelset_(w);
          if (is_.peek() == '_' || is_.peek() == '-')
            return context_(res);
          return res;
        }
      else if (w == "lat")
        {
          std::shared_ptr<tupleset> res = tupleset_();
          if (is_.peek() == '_' || is_.peek() == '-')
            return context_(res);
          return res;
        }
      else if (w == "b"
               || w == "f2"
               || w == "q"
               || w == "qmp"
               || w == "r"
               || w == "z"
               || w == "zmin")
        return weightset_(w);
      else if (w == "polynomialset")
        {
          eat(is_, '<');
          auto res = std::make_shared<polynomialset>(context_());
          eat(is_, '>');
          return res;
        }
      else if (w == "std::integral")
        {
          // std::integral_constant<unsigned, 2>.
          w += eat(is_, "_constant<");
          w += word();
          w += eat(is_, ',');
          w += word();
          w += eat(is_, '>');
          return std::make_shared<other>(w);
        }
      else
        return std::make_shared<other>(w);
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_or_weightset_()
    {
      return labelset_or_weightset_(word());
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_or_weightset_(const std::string& w)
    {
      if (w == "lat")
        return tupleset_();
      else if (w == "ratexpset")
        return ratexpset_();
      else if (w == "seriesset")
        return ratexpset_series_();
      else if (w == "lal"
               || w == "lan"
               || w == "lao"
               || w == "law")
        return labelset_(w);
      else if (w == "b"
               || w == "f2"
               || w == "q"
               || w == "qmp"
               || w == "r"
               || w == "z"
               || w == "zmin")
        return weightset_(w);
      else
        raise("invalid weightset or labelset name: " + w);
    }

    std::string context_parser::word()
    {
      std::string res;
      int c;
      while (is_.peek() == ' ')
        is_.get();
      while ((c = is_.get()) != EOF)
        if (c == '<' || c == ',' || c == '_' || c == '>' || c == '-')
          {
            is_.unget();
            break;
          }
        else
          res += c;
      return res;
    }

    std::string context_parser::alphabet_()
    {
      std::string res;
      if (is_.peek() != '(')
        return res;
      res += '(';
      int c = is_.get();
      while ((c = is_.get()) != EOF)
        {
          res += c;
          if (c == ')')
            break;
        }
      return res;
    }

    std::shared_ptr<context> context_parser::context_()
    {
      return context_(word());
    }

    std::shared_ptr<context>
    context_parser::context_(const std::shared_ptr<ast_node>& ls)
    {
      if (is_.peek() == '_')
        eat(is_, '_');
      else
        eat(is_, "->");
      return std::make_shared<context>(ls, weightset_());
    }

    std::shared_ptr<context>
    context_parser::context_(const std::string& word)
    {
      return context_(labelset_(word));
    }

    std::shared_ptr<ast_node> context_parser::labelset_()
    {
      return labelset_(word());
    }

    std::shared_ptr<ast_node>
    context_parser::labelset_(const std::string& kind)
    {
      if (kind == "lal" || kind == "law")
        {
          eat(is_, "_char");
          if (kind == "lal")
            return std::make_shared<letterset>(alphabet_());
          else if (kind == "law")
            return std::make_shared<wordset>(alphabet_());
        }
      else if (kind == "lao")
        return std::make_shared<oneset>();
      else if (kind == "lan")
        {
          if (is_.peek() == '_')
            {
              eat(is_, "_char");
              return std::make_shared<nullableset>(std::make_shared<letterset>
                                                   (alphabet_()));
            }
          else
            {
              eat(is_, '<');
              auto res = labelset_();
              if (!res->has_one())
                res = std::make_shared<nullableset>(res);
              eat(is_, '>');
              return res;
            }
        }
      else if (kind == "lat")
        return tupleset_();
      else if (kind == "ratexpset")
        return ratexpset_();
      else if (kind == "seriesset")
        return ratexpset_series_();
      raise("invalid labelset name: ", str_escape(kind));
    }

    std::shared_ptr<ast_node> context_parser::weightset_()
    {
      return weightset_(word());
    }

    std::shared_ptr<ast_node>
    context_parser::weightset_(const std::string& ws)
    {
      if (ws == "b" || ws == "f2" || ws == "q" || ws == "qmp"
          || ws == "r" || ws == "z" || ws == "zmin")
        return std::make_shared<weightset>(ws);
      else if (ws == "ratexpset")
        return ratexpset_();
      else if (ws == "seriesset")
        return ratexpset_series_();
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
      // blind_automaton<TapeNum, Aut>.
      if (prefix == "blind")
        {
          prefix += eat(is_, "_automaton");
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            std::make_shared<other>(word()));
          eat(is_, ',');
          res->get_content().emplace_back(automaton_(word()));
        }
      // xxx_automaton<Aut>.
      else if (prefix == "determinized"
               || prefix == "detweighted"
               || prefix == "pair"
               || prefix == "partition"
               || prefix == "permutation"
               || prefix == "ratexp"
               || prefix == "sub"
               || prefix == "transpose")
        {
          prefix += eat(is_, "_automaton");
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            automaton_(word()));
        }
      // mutable_automaton<Context>.
      else if (prefix == "mutable")
        {
          prefix += eat(is_, "_automaton");
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            context_());
        }
      // xxx_automaton<Aut...>.
      else if (prefix == "product"
               || prefix == "tuple")
        {
          prefix += eat(is_, "_automaton");
          eat(is_, '<');
          res = std::make_shared<automaton>(prefix,
                                            automaton_(word()));
          while (is_.peek() == ',')
            {
              eat(is_, ',');
              res->get_content().emplace_back(automaton_(word()));
            }
        }
      else
        raise("invalid automaton name: ", str_escape(prefix));
      eat(is_, '>');
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

    std::shared_ptr<ratexpset>
    context_parser::ratexpset_()
    {
      eat(is_, '<');
      auto context = context_();
      eat(is_, '>');
      rat::identities identities = rat::identities::trivial;
      if (is_.peek() == '(')
        {
          eat(is_, '(');
          is_ >> identities;
          eat(is_, ')');
        }
      return std::make_shared<ratexpset>(context, identities);
    }

    std::shared_ptr<ratexpset>
    context_parser::ratexpset_series_()
    {
      eat(is_, '<');
      auto context = context_();
      eat(is_, '>');
      return std::make_shared<ratexpset>(context, rat::identities::series);
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
