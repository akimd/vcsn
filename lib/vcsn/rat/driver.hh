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

      /// Get the context.
      dyn::context context() const;
      /// Set the expressionset to use from its context name.
      void context(const std::string& ctx);

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

    private:
      /// The inital location.
      location location_;
      dyn::context ctx_;
      rat::identities ids_;
      dyn::expression result_;
      friend class parser;
    };

  }
}
