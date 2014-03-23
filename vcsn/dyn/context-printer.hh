#ifndef VCSN_DYN_CONTEXT_PRINTER_HH
# define VCSN_DYN_CONTEXT_PRINTER_HH

# include <set>
# include <sstream>

# include <vcsn/dyn/context-visitor.hh>

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
      void header(std::string h);

      /// Generate the code to compile on \a o.
      std::ostream& print(std::ostream& o);

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

      std::ostringstream& os_;
        /// Headers to include.
        ///
        /// Sadly enough functions about tupleset must be defined
        /// after the functions that define the behavior of the
        /// components.  The genuine case is that of "print_set",
        /// which fails for the same reasons as the following does not
        /// compile:
        ///
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
        ///
        /// So we use a second set for "late" headers.
      std::set<std::string> headers_;
      std::set<std::string> headers_late_;
    };
  }
}

#endif /* !VCSN_DYN_CONTEXT_PRINTER_HH */
