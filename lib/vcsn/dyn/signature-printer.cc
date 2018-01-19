#include <sstream>

#include <lib/vcsn/dyn/context-parser.hh>
#include <lib/vcsn/dyn/signature-printer.hh>


namespace vcsn::ast
{
  namespace
  {
    /// Print a type ast in a user friendly manner.
    ///
    /// This is also the format which is used to name the generated
    /// files.
    class signature_printer : public visitor
    {
    public:
      /// Construct the printer.
      ///
      /// \param os  output stream
      /// \param full  whether to print the dynamic values
      ///      (e.g., `lal(abc), b` instead of `lal, b`.
      signature_printer(std::ostream& os, bool full)
        : os_(os), full_(full)
      {}

#define DEFINE(Type)                            \
    void visit(const Type& t)

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
      os_ << "context<";
      t.get_labelset()->accept(*this);
      os_ << ", ";
      t.get_weightset()->accept(*this);
      os_ << ">";
    }

    DEFINE(tuple)
    {
      os_ << "std::tuple<";
      auto v = t.get_sets();
      for (unsigned int i = 0; i < v.size() - 1; ++i)
      {
        v[i]->accept(*this);
        os_ << ", ";
      }
      v[v.size() - 1]->accept(*this);
      os_ << '>';
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

    DEFINE(oneset)
    {
      (void) t;
      os_ << "oneset";
    }

    DEFINE(genset)
    {
      os_ << t.letter_type();
      if (full_)
        os_ << t.generators();
    }

    DEFINE(genlabelset)
    {
      if (t.type() == "letterset")
        {
          os_ << "letterset<";
          t.genset()->accept(*this);
          os_ << '>';
        }
      else
        {
          os_ << "wordset<";
          t.genset()->accept(*this);
          os_ << '>';
        }
    }

    DEFINE(expansionset)
    {
      os_ << "expansionset<";
      t.get_expressionset()->accept(*this);
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

#undef DEFINE

    private:
      std::ostream& os_;
      bool full_;
    };
  }


  std::string pretty(const std::shared_ptr<ast_node>& t, bool full)
  {
    auto o = std::ostringstream{};
    auto pr = signature_printer{o, full};
    t->accept(pr);
    return o.str();
  }

  std::string normalize_context(const std::string& ctx, bool full)
  {
    return pretty(parse_context(ctx), full);
  }
}
