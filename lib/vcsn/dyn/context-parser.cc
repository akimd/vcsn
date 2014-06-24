#include <vcsn/dyn/context-parser.hh>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace ast
  {

    std::shared_ptr<ast_node> context_parser::parse_()
    {
      std::string w = word();
      if (w == "blind"
          || w == "determinized"
          || w == "mutable"
          || w == "pair"
          || w == "product"
          || w == "ratexp"
          || w == "subset"
          || w == "transpose"
          || w == "tuple")
        return make_automaton(w);
      else if (w == "context")
      {
        eat(is_, '<');
        std::shared_ptr<context> res = make_context();
        eat(is_, '>');
        return res;
      }
      else if (w == "lal"
            || w == "lan"
            || w == "lao"
            || w == "law"
            || w == "ratexpset")
      {
        std::shared_ptr<ast_node> res = make_labelset(w);
        if (is_.peek() == '_')
        {
          eat(is_, '_');
          return std::make_shared<context>(res, make_weightset());
        }
        return res;
      }
      else if (w == "lat")
      {
        std::shared_ptr<tupleset> res = make_tupleset();
        if (is_.peek() == '_')
        {
          eat(is_, '_');
          return std::make_shared<context>(res, make_weightset());
        }
        return res;
      }
      else if (w == "b"
            || w == "f2"
            || w == "q"
            || w == "r"
            || w == "z"
            || w == "zmin")
        return make_weightset(w);
      else if (w == "polynomialset")
      {
        eat(is_, '<');
        std::shared_ptr<polynomialset> res =
          std::make_shared<polynomialset>(make_context());
        eat(is_, '>');
        return res;
      }
      else
        return std::make_shared<other>(w);
    }

    std::shared_ptr<ast_node> context_parser::parse()
    {
      std::shared_ptr<ast_node> res = parse_();
      if (is_.peek() != -1)
        vcsn::fail_reading(is_, "unexpected trailing characters");
      return res;
    }

    std::shared_ptr<ast_node> context_parser::make_labelset_or_weightset()
    {
      return make_labelset_or_weightset(word());
    }

    std::shared_ptr<ast_node>
    context_parser::make_labelset_or_weightset(const std::string& w)
    {
      if (w == "lal"
       || w == "lan"
       || w == "lao"
       || w == "law"
       || w == "lat"
       || w == "ratexpset")
        return make_labelset(w);
      else if (w == "b"
            || w == "f2"
            || w == "q"
            || w == "r"
            || w == "z"
            || w == "zmin")
        return make_weightset(w);
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
        if (c == '<' || c == ',' || c == '_' || c == '>')
          {
            is_.unget();
            break;
          }
        else
          res += c;
      return res;
    }

    std::string context_parser::get_alpha()
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

    std::shared_ptr<context> context_parser::make_context()
    {
      return make_context(word());
    }

    std::shared_ptr<context>
    context_parser::make_context(const std::string& word)
    {
      std::shared_ptr<ast_node> ls = make_labelset(word);
      eat(is_, '_');
      return std::make_shared<context>(ls, make_weightset());
    }

    std::shared_ptr<ast_node> context_parser::make_labelset()
    {
      return make_labelset(word());
    }

    std::shared_ptr<ast_node>
    context_parser::make_labelset(const std::string& kind)
    {
      if (kind == "lal" || kind == "law")
      {
        eat(is_, "_char");
        if (kind == "lal")
          return std::make_shared<letterset>(get_alpha());
        else if (kind == "law")
          return std::make_shared<wordset>(get_alpha());
      }
      else if (kind == "lao")
        return std::make_shared<oneset>();
      else if (kind == "lan")
      {
        if (is_.peek() == '_')
        {
          eat(is_, "_char");
          return std::make_shared<nullableset>(std::make_shared<letterset>
                                               (get_alpha()));
        }
        else
        {
          eat(is_, '<');
          std::shared_ptr<ast_node> res = make_labelset();
          if (!res->has_one())
            res = std::make_shared<nullableset>(res);
          eat(is_, '>');
          return res;
        }
      }
      else if (kind == "lat")
        return make_tupleset();
      else if (kind == "ratexpset")
        return make_ratexpset();
      raise("invalid labelset name: ", str_escape(kind));
    }

    std::shared_ptr<ast_node> context_parser::make_weightset()
    {
      return make_weightset(word());
    }

    std::shared_ptr<ast_node>
    context_parser::make_weightset(const std::string& ws)
    {
      if (ws == "b" || ws == "f2"  || ws == "q"
          || ws == "r" || ws == "z" || ws == "zmin")
        return std::make_shared<weightset>(ws);
      else if (ws == "ratexpset")
        return make_ratexpset();
      else if (ws == "polynomialset")
        return make_polynomialset();
      else if (ws == "lat")
        return make_tupleset();
      else
        raise("invalid weightset name: ", str_escape(ws));
    }

    std::shared_ptr<automaton>
    context_parser::make_automaton(const std::string& prefix)
    {
      std::shared_ptr<automaton> res = nullptr;
      // blind_automaton<TapeNum, Aut>.
      if (prefix == "blind")
        {
          eat(is_, "_automaton<");
          res = std::make_shared<automaton>(prefix + "_automaton",
                                            std::make_shared<other>(word()));
          eat(is_, ',');
          res->get_content().emplace_back(make_automaton(word()));
        }
      // xxx_automaton<Aut>.
      else if (prefix == "determinized"
               || prefix == "linear"
               || prefix == "pair"
               || prefix == "ratexp"
               || prefix == "subset"
               || prefix == "transpose")
        {
          eat(is_, "_automaton<");
          res = std::make_shared<automaton>(prefix + "_automaton",
                                            make_automaton(word()));
        }
      // mutable_automaton<Context>.
      else if (prefix == "mutable")
        {
          eat(is_, "_automaton<");
          res = std::make_shared<automaton>(prefix + "_automaton",
                                            make_context());
        }
      // xxx_automaton<Aut...>.
      else if (prefix == "product"
               || prefix == "tuple")
        {
          eat(is_, "_automaton<");
          res = std::make_shared<automaton>(prefix + "_automaton",
                                            make_automaton(word()));
          while (is_.peek() == ',')
            {
              eat(is_, ',');
              res->get_content().emplace_back(make_automaton(word()));
            }
        }
      else
        raise("invalid automaton name: ", str_escape(prefix));
      eat(is_, '>');
      return res;
    }

    std::shared_ptr<tupleset> context_parser::make_tupleset()
    {
      eat(is_, '<');
      typename tupleset::value_t res;
      res.emplace_back(make_labelset_or_weightset());
      while (is_.peek() == ',')
      {
        eat(is_, ',');
        res.emplace_back(make_labelset_or_weightset());
      }
      eat(is_, '>');
      return std::make_shared<tupleset>(res);
    }

    std::shared_ptr<ratexpset> context_parser::make_ratexpset()
    {
      eat(is_, '<');
      std::shared_ptr<ratexpset> res =
        std::make_shared<ratexpset>(make_context());
      eat(is_, '>');
      return res;
    }

    std::shared_ptr<polynomialset> context_parser::make_polynomialset()
    {
      eat(is_, '<');
      std::shared_ptr<polynomialset> res =
        std::make_shared<polynomialset>(make_context());
      eat(is_, '>');
      return res;
    }
  }
}
