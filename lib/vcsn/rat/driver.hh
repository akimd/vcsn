#pragma once

#include <lib/vcsn/rat/fwd.hh>
#include <lib/vcsn/rat/location.hh>
#include <lib/vcsn/rat/parse.hh>
#include <vcsn/core/rat/fwd.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace rat
  {
    /// State and public interface for rational expression parsing.
    class LIBVCSN_API driver
    {
    public:
      driver(const dyn::context& ctx, rat::identities ids);
      ~driver();

      /// Set the expressionset to use from this context.
      void context(const dyn::context& ctx);

      /// Set the expressionset to use from its context name.
      void context(const std::string& ctx);

      /// Get the current context.
      /// Depends on the current tape.
      dyn::context context() const;

      /// Get the identities.
      rat::identities identities() const;

      /// Push a new tape number on the stack.
      void tape_push();

      /// Pop the tape stack.
      void tape_pop();

      /// Increment the top most tape.
      void tape_inc(const location& l);

      /// Parse this stream.
      dyn::expression parse(std::istream& is, const location& l = location{});

      /// Report an error \a m at \a l.
      void error(const location& l, const std::string& m);
      /// The string \a s is invalid at \a l.
      void invalid(const location& l, const std::string& s);

      /// The error messages.
      std::string errors;
      /// The scanner.
      std::unique_ptr<yyFlexLexer> scanner_;

      /// From a string, generate a label.
      dyn::label make_label(const location& loc, const std::string& s,
                            const dyn::context& ctx);

      /// From a string, generate an expression.
      dyn::expression make_atom(const location& loc, const std::string& s);

      /// From a label class, generate an expression.
      dyn::expression make_expression(const location& loc,
                                      const class_t& c, bool accept);

      /// From a string, generate a weight.
      dyn::weight make_weight(const location& loc, const std::string& s);

    private:
      friend class parser;

      /// The inital location.
      location location_;
      /// The context used to read the expression.
      dyn::context ctx_;
      /// The identities to apply.
      rat::identities ids_;
      /// The parsed expression.
      dyn::expression result_;
      /// The stack of tape numbers.
      ///
      /// The concept of tape number does not suffice.  For instance,
      /// it cannot deal with `(a|b)|(x|y)` which is a `lat<lat<lal,
      /// lal>, lat<lal, lal>>`.  Not that it really matters as of
      /// today...
      std::vector<unsigned> tapes_ = {0};
      /// The context for each tape.  If single-tape, [0] is ctx_.
      std::vector<dyn::context> tape_ctx_ = {};
    };
  }
}
