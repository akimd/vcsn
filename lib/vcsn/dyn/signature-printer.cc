#include <ostream>

#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/signature-printer.hh>


namespace vcsn
{
  namespace ast
  {

    std::string normalize(const std::string& sig)
    {
      std::ostringstream os;
      ast::signature_printer printer(os, true);
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
      t.get_content()->accept(*this);
      os_ << '>';
    }

    DEFINE(context)
    {
      t.get_labelset()->accept(*this);
      os_ << '_';
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

    DEFINE(letterset)
    {
      os_ << "lal_char";
      if (full_)
        os_ << t.get_alpha();
    }

    DEFINE(ratexpset)
    {
      os_ << "ratexpset<";
      t.get_context()->accept(*this);
      os_ << '>';
    }

    DEFINE(weightset)
    {
      os_ << t.get_type();
    }

    DEFINE(wordset)
    {
      os_ << "law_char";
      if (full_)
        os_ << t.get_alpha();
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
