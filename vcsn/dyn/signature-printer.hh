#pragma once

#include <ostream>

#include <vcsn/dyn/type-ast.hh>

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

#define DEFINE(Type)                            \
      virtual void visit(const Type& t)

      DEFINE(automaton);
      DEFINE(context);
      DEFINE(genset);
      DEFINE(letterset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(other);
      DEFINE(polynomialset);
      DEFINE(expressionset);
      DEFINE(tupleset);
      DEFINE(weightset);
      DEFINE(wordset);

#undef DEFINE

    private:
      std::ostream& os_;
      bool full_;
    };

    /// The signature, normalized.
    ///
    /// E.g., 'lan_char(ab), b' -> 'lan<letterset<char_letters(ab)>, b'.
    ///
    /// \param sig   the signature to normalize.
    /// \param full  whether to keep the runtime values
    ///    (e.g., the generators: 'lan<letterset<char_letters(ab)>, b' or
    ///                           'lan<letterset<char_letters>, b').
    std::string normalize(const std::string& sig, bool full = true);

  }
}
