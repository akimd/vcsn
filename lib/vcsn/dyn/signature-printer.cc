#include <ostream>

#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/signature-printer.hh>


namespace vcsn
{
  namespace ast
  {

    std::string normalize(const std::string& sig, bool full)
    {
      std::ostringstream os;
      ast::signature_printer printer(os, full);
      std::istringstream is;
      ast::context_parser parser(is);
      is.str(sig);
      auto ast = parser.parse();
      ast->accept(printer);
      return os.str();
    }

#define DEFINE(Type)                            \
    void signature_printer::visit(const Type& t)

    DEFINE(automaton)
    {
      os_ << t.get_type() << '<';
      bool first = true;
      for (auto c: t.get_content())
        {
          if (!first)
            os_ << ", ";
          first = false;
          c->accept(*this);
        }
      os_ << '>';
    }

    DEFINE(context)
    {
      t.get_labelset()->accept(*this);
      os_ << ", ";
      t.get_weightset()->accept(*this);
    }

    DEFINE(tupleset)
    {
      os_ << "lat<";
      auto v = t.get_sets();
      for (unsigned int i = 0; i < v.size() - 1; ++i)
      {
        v[i]->accept(*this);
        os_ << ", ";
      }
      v[v.size() - 1]->accept(*this);
      os_ << '>';
    }

    DEFINE(nullableset)
    {
      os_ << "lan<";
      t.get_labelset()->accept(*this);
      os_ << '>';
    }

    DEFINE(oneset)
    {
      (void) t;
      os_ << "lao";
    }

    DEFINE(genset)
    {
      os_ << t.letter_type();
      if (full_)
        os_ << t.generators();
    }

    DEFINE(letterset)
    {
      os_ << "letterset<";
      t.genset()->accept(*this);
      os_ << '>';
    }

    DEFINE(expressionset)
    {
      os_ << "expressionset<";
      t.get_context()->accept(*this);
      os_ << '>';
      if (full_)
        os_ << '(' << t.get_identities() << ')';
    }

    DEFINE(weightset)
    {
      os_ << t.get_type();
    }

    DEFINE(wordset)
    {
      os_ << "wordset<";
      t.genset()->accept(*this);
      os_ << '>';
    }

    DEFINE(other)
    {
      os_ << t.get_type();
    }

    DEFINE(polynomialset)
    {
      os_ << "polynomialset<";
      t.get_content()->accept(*this);
      os_ << '>';
    }

  }
}
