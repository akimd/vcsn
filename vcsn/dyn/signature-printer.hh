#ifndef VCSN_DYN_SIGNATURE_PRINTER_HH
# define VCSN_DYN_SIGNATURE_PRINTER_HH

# include <ostream>

# include <vcsn/dyn/type-ast.hh>

namespace vcsn
{
  namespace ast
  {
    class signature_printer : public context_visitor
    {
    public:
      signature_printer(std::ostream& os, bool full)
        : os_(os), full_(full)
      {}

# define DEFINE(Type)                   \
      virtual void visit(const Type& t)

      DEFINE(automaton);
      DEFINE(context);
      DEFINE(tupleset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(letterset);
      DEFINE(ratexpset);
      DEFINE(weightset);
      DEFINE(wordset);
      DEFINE(other);
      DEFINE(polynomialset);

# undef DEFINE

    private:
      std::ostream& os_;
      bool full_;
    };

    std::string normalize(const std::string& sig);

  }
}

#endif // !VCSN_DYN_SIGNATURE_PRINTER_HH
