#pragma once

#include <set>
#include <sstream>

#include <lib/vcsn/dyn/context-visitor.hh>

namespace vcsn
{
  namespace ast
  {
    class context_printer : public context_visitor
    {
    public:
      context_printer(std::ostringstream& os)
        : os_(os)
      {}

      /// Record that we need an include for this header.
      void header(const std::string& h);

      /// Record that we need an include for this algorithm.
      void header_algo(const std::string& algo);

      /// Record that we need these linker flags.
      void linkflags(const std::string& h);

      /// Get the link flags.
      const std::string& linkflags() const;

      /// Generate the code to compile on \a o.
      std::ostream& print(std::ostream& o);

# define DEFINE(Type)                   \
      virtual void visit(const Type& t)

      DEFINE(automaton);
      DEFINE(automatonset);
      DEFINE(context);
      DEFINE(expansionset);
      DEFINE(expressionset);
      DEFINE(genset);
      DEFINE(letterset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(other);
      DEFINE(polynomialset);
      DEFINE(tuple);
      DEFINE(tupleset);
      DEFINE(weightset);
      DEFINE(wordset);
# undef DEFINE

    private:

      std::ostringstream& os_;
      /// Headers to include.
      ///
      /// Sadly enough functions about tupleset must be defined
      /// after the functions that define the behavior of the
      /// components.  The genuine case is that of "print_set",
      /// which fails for the same reasons as the following does not
      /// compile:
      ///
      /// @code
      /// template <typename T>
      /// struct wrapper
      /// {
      ///   T t;
      /// };
      ///
      /// template <typename T>
      /// void print(const wrapper<T>& w)
      /// {
      ///   print(w.t);
      /// }
      ///
      /// void print(int){}
      ///
      /// int main()
      /// {
      ///   wrapper<int> w;
      ///   print(w);
      /// }
      /// @endcode
      ///
      /// So we use a second set for "late" headers.
      std::set<std::string> headers_;
      std::set<std::string> headers_late_;

      /// Flags to pass to the linker.
      std::string linkflags_;
    };
  }
}
