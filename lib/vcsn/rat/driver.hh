#pragma once

#include <vcsn/core/rat/fwd.hh>
#include <vcsn/dyn/expression.hh>
#include <lib/vcsn/rat/location.hh>
#include <vcsn/misc/export.hh>

#include <lib/vcsn/rat/fwd.hh>
#include <lib/vcsn/rat/parse.hh>

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

      /// Set the expressionset to use from its context name.
      void context(const std::string& ctx);

      /// Get the current context.
      /// Depends on the current tape.
      dyn::context context() const;

      /// Get the identities.
      rat::identities identities() const;

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
      dyn::label make_label(const location& loc, const std::string& s);

      /// From a string, generate an expression.
      dyn::expression make_atom(const location& loc, const std::string& s);

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
      /// The current tape number.
      unsigned tape_ = 0;
    };
  }
}
