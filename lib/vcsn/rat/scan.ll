%option c++
%option debug
%option noinput
%option stack noyy_top_state

%top{
// Define YY_DECL.
#include <lib/vcsn/rat/parse.hh>

#define yyterminate() return parser::make_END(loc)
}

%{
#include <cassert>
#include <iostream>
#include <stack>
#include <string>

#include <boost/lexical_cast.hpp>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/regex.hh>
#include <vcsn/misc/string.hh>

#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>

#define LINE(Line)                              \
  do {                                          \
    loc.end.column = 1;                         \
    loc.lines(Line);                            \
 } while (false)

#define YY_USER_ACTION                          \
  loc.columns(length(yytext, yytext+yyleng));

#define TOK(Token)                              \
  parser::make_ ## Token (loc)

YY_FLEX_NAMESPACE_BEGIN
namespace
{
  irange_type quantifier(driver& d, const location& loc, const std::string& s);
}
%}

%x SC_CLASS SC_CONTEXT SC_ERE SC_EXPONENT SC_WEIGHT

 /* Abbreviations. */
id   [a-zA-Z][a-zA-Z_0-9]*

%%
%{
  // Count the number of opened braces in SC_WEIGHT, and parens in SC_CONTEXT.
  unsigned int nesting = 0;
  // Compute the exponent.
  unsigned int exponent = 0;
  // Build a context string.  "static" only to save build/dtor.
  static std::string context;
  std::string s;
  loc.step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "("         return parser::make_LPAREN({}, loc);
  ")"         return TOK(RPAREN);
  "]"         return TOK(RBRACKET);

  "&"         return TOK(AMPERSAND);
  ":"         return TOK(COLON);
  "&:"        return TOK(AMPERSAND_COLON);
  "%"         return TOK(PERCENT);
  "+"         return TOK(PLUS);
  "<+"        return TOK(LT_PLUS);
  "."         return TOK(DOT);
  "{\\}"      return TOK(BACKSLASH);
  "{/}"       return TOK(SLASH);
  ","         return TOK(COMMA);
  "|"         return TOK(PIPE);
  "@"         return TOK(AT);
  "\\z"|"∅"   return TOK(ZERO);
  "\\e"|"ε"   return TOK(ONE);

  /* Quantifiers.  */
  "?"|"{?}"            return parser::make_STAR({0, 1}, loc);
  "*"|"∗"|"{*}"        return parser::make_STAR({-1, -1}, loc);
  "{+}"                return parser::make_STAR({1, -1}, loc);
  "{"[0-9]*,?[0-9]*"}" {
    return parser::make_STAR(quantifier(driver_, loc,
                                        {yytext+1, static_cast<size_t>(yyleng)-2}),
                             loc);
  }
  "⁰"|"¹"|"²"|"³"|"⁴"|"⁵"|"⁶"|"⁷"|"⁸"|"⁹" {
    // Detect an exponent, do not read it yet. Handle it in the exponent state.
    yyless(0);
    loc -= yyleng;
    exponent = 0;
    yy_push_state(SC_EXPONENT);
  }


  "!"|"¬"     return TOK(BANG);
  "{c}"|"ᶜ"   return TOK(COMPLEMENT);
  "{T}"|"ᵗ"   return TOK(TRANSPOSITION);
  "{t}"       return TOK(TRANSPOSE);

  /* Special constructs.  */
  "(?@"          context.clear(); yy_push_state(SC_CONTEXT);
  "(?<"[^<>]+">"   {
    return parser::make_LPAREN(symbol{yytext + 3, yyleng - 4},
                               loc);
  }
  "(?#"[^\)]*")"  continue;

  /* Weights. */
  "<"|"⟨"     yy_push_state(SC_WEIGHT);

  /* Character classes.  */
  "["     yy_push_state(SC_CLASS); return TOK(LBRACKET);

  /* White spaces. */
  [ \t]+   loc.step(); continue;
  "\n"+    LINE(yyleng); loc.step(); continue;

  [{}>]|"⟩"    {
    throw parser::syntax_error(loc,
                               std::string{"unexpected character: "} + yytext);
  }
}

<SC_ERE>{ /* Syntax of grep -E.  */
  "("         return parser::make_LPAREN({}, loc);
  ")"         return TOK(RPAREN);
  "]"         return TOK(RBRACKET);

  "|"         return TOK(PLUS);

  /* Quantifiers.  */
  "?"        return parser::make_STAR({0, 1}, loc);
  "*"        return parser::make_STAR({-1, -1}, loc);
  "+"        return parser::make_STAR({1, -1}, loc);
  "{"[0-9]*,?[0-9]*"}" {
    return parser::make_STAR(quantifier(driver_, loc,
                                        {yytext+1, static_cast<size_t>(yyleng)-2}),
                             loc);
  }

  /* Special constructs.  */
  "(?#"[^\)]*")"  continue;

  /* Character classes.  */
  "["     yy_push_state(SC_CLASS); return TOK(LBRACKET);

  \\[0-7]{3}        |
  \\x[0-9a-fA-F]{2} |
  "\\"[\\()\[\]]    |
  .                 return parser::make_LETTER({yytext, size_t(yyleng)}, loc);
}

<SC_CLASS>{ /* Character-class.  Initial [ is eaten. */
  "]" {
    yy_pop_state();
    return TOK(RBRACKET);
  }
  "^" return TOK(CARET);
  "-" return TOK(DASH);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a character-class");
    unput(']');
  }
}


<INITIAL,SC_CLASS>{ /* Labels.  */
  "'"([^\']|\\.)+"'"  {
    return parser::make_LETTER({yytext+1, size_t(yyleng-2)}, loc);
  }

  \\[0-7]{3}        |
  \\x[0-9a-fA-F]{2} |
  "\\".             |
  .                 return parser::make_LETTER({yytext, size_t(yyleng)}, loc);
}


<SC_CONTEXT>{ /* Context embedded in a $(?@...) directive.  */
  "("   {
    ++nesting;
    context.append(yytext, yyleng);
  }
  ")"   {
    if (nesting)
      {
        --nesting;
        context.append(yytext, yyleng);
      }
    else
      {
        yy_pop_state();
        driver_.context(context);
        context.clear();
      }
  }
  [^()\n]+   context.append(yytext, yyleng);
  \n+        LINE(yyleng); context.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a context comment");
    unput(')');
  }
}

<SC_EXPONENT>{ /* UTF-8 Exponent */
  "⁰" exponent *= 10;
  "¹" exponent *= 10; exponent += 1;
  "²" exponent *= 10; exponent += 2;
  "³" exponent *= 10; exponent += 3;
  "⁴" exponent *= 10; exponent += 4;
  "⁵" exponent *= 10; exponent += 5;
  "⁶" exponent *= 10; exponent += 6;
  "⁷" exponent *= 10; exponent += 7;
  "⁸" exponent *= 10; exponent += 8;
  "⁹" exponent *= 10; exponent += 9;
  .|"\n" {
    // The end of the exponent was met, do not read it as it would be lost.
    yyless(0);
    loc -= yyleng;
    yy_pop_state();
    return parser::make_STAR({exponent, exponent}, loc);
  }
  <<EOF>> {
    yy_pop_state();
    return parser::make_STAR({exponent, exponent}, loc);
  }
}

<SC_WEIGHT>{ /* Weight.  */
  "<"|"⟨"                       {
    ++nesting;
    s.append(yytext, yyleng);
  }

  ">"|"⟩"                       {
    if (nesting)
      {
        --nesting;
        s.append(yytext, yyleng);
      }
    else
      {
        yy_pop_state();
        return parser::make_WEIGHT(s, loc);
      }
  }

  /* "⟨" is e2 9f a8, and "⟩" is e2 9f a9. */
  [^<>\xe2]+|"\xe2"      s.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a weight");
    unput('>');
  }
}

%%
namespace
{
  /// Safe conversion to a numeric value.
  template <typename Out>
  Out
  lexical_cast(driver& d, const location& loc, const std::string& s)
  {
    try
      {
        return boost::lexical_cast<Out>(s);
      }
    catch (const boost::bad_lexical_cast&)
      {
        d.error(loc, "invalid numerical literal: " + s);
        return 0;
      }
  }

  /// The value of s, a decimal number, or -1 if empty.
  int arity(driver& d, const location& loc, const std::string& s)
  {
    return s.empty() ? -1 : lexical_cast<int>(d, loc, s);
  }

  /// Decode a quantifier's value: "1,2" etc.
  ///
  /// We used to include the braces in \a s, but a libc++ bug in regex
  /// made the following regex unportable.
  /// http://llvm.org/bugs/show_bug.cgi?id=16135
  irange_type
  quantifier(driver& d, const location& loc, const std::string& s)
  {
    auto arity_re = std::regex{"([0-9]*)(,?)([0-9]*)", std::regex::extended};
    auto minmax = std::smatch{};
    if (!std::regex_match(s, minmax, arity_re))
      throw std::runtime_error("cannot match arity: " + s);
    auto res = irange_type{arity(d, loc, minmax[1].str()),
                           arity(d, loc, minmax[3].str())};
    if (minmax[2].str().empty())
      // No comma: single argument.
      std::get<1>(res) = std::get<0>(res);
    return res;
  }
}

// Do not use %option noyywrap, because then flex generates the
// same definition of yywrap, but outside the namespaces, so it
// defines it for ::yyFlexLexer instead of ::vcsn::rat::yyFlexLexer.
int yyFlexLexer::yywrap() { return 1; }

// Beware of the dummy Flex interface.  One would like to use:
//
// yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
//
// and
//
// yypush_buffer_state(yy_scan_bytes(e.c_str(), e.size()));
//
// but the latter (yy_scan_bytes) calls yy_switch_to_buffer, so in
// effect calling yypush_buffer_state saves the new state instead
// of the old one.
//
// So do it in two steps, quite different from what is suggested
// in the documentation: save the old context, switch to the new
// one.

void yyFlexLexer::scan_open_(std::istream& f)
{
  set_debug(!!getenv("YYSCAN"));
  yypush_buffer_state(YY_CURRENT_BUFFER);
  if (driver_->fmt_ == format::ere)
    yy_push_state(SC_ERE);
  yy_switch_to_buffer(yy_create_buffer(&f, YY_BUF_SIZE));
}

void yyFlexLexer::scan_close_()
{
  yypop_buffer_state();
}

YY_FLEX_NAMESPACE_END

// Local Variables:
// mode: C++
// End:
