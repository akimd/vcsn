%option c++
%option debug
%option noinput nounput
%option stack noyy_top_state

%top{
#pragma GCC diagnostic ignored "-Wsign-compare"
   // Check Clang first, as it does not support -Wzero... but it
   // defines __GNUC__.
#if defined __clang__
# pragma clang diagnostic ignored "-Wdeprecated"
# pragma clang diagnostic ignored "-Wnull-conversion"
#elif defined __GNUC__
# pragma GCC diagnostic ignored "-Wsuggest-attribute=const"
# pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
# pragma GCC diagnostic ignored "-Wsuggest-attribute=pure"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

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

#include <vcsn/misc/regex.hh>
#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>

#define LINE(Line)                              \
  do{                                           \
    loc.end.column = 1;                         \
    loc.lines(Line);                            \
 } while (false)

#define YY_USER_ACTION                          \
  loc.columns(yyleng);

#define TOK(Token)                              \
  parser::make_ ## Token (loc)

YY_FLEX_NAMESPACE_BEGIN
namespace
{
  irange_type quantifier(driver& d, const location& loc, const std::string& s);
}
%}

%x SC_CLASS SC_CONTEXT SC_WEIGHT

char      ([a-zA-Z0-9_]|\\[<>{}()+.*:\"])

%%
%{
  // Count the number of opened braces in SC_WEIGHT, and parens in SC_CONTEXT.
  unsigned int nesting = 0;
  // Build a context string.  "static" only to save build/dtor.
  static std::string context;
  std::string s;
  loc.step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "("     return TOK(LPAREN);
  ")"     return TOK(RPAREN);

  "&"     return TOK(AMPERSAND);
  ":"     return TOK(COLON);
  "%"     return TOK(PERCENT);
  "+"     return TOK(PLUS);
  "."     return TOK(DOT);
  "{\\}"  return TOK(BACKSLASH);
  "{/}"   return TOK(SLASH);
  ","     return TOK(COMMA);
  "\\e"   return TOK(ONE);
  "\\z"   return TOK(ZERO);

  /* Quantifiers.  */
  "?"|"{?}"            return parser::make_STAR(std::make_tuple(0, 1), loc);
  "*"|"{*}"            return parser::make_STAR(std::make_tuple(-1, -1), loc);
  "{+}"                return parser::make_STAR(std::make_tuple(1, -1), loc);
  "{"[0-9]*,?[0-9]*"}" {
    return parser::make_STAR(quantifier(driver_, loc,
                                        {yytext+1, static_cast<size_t>(yyleng)-2}),
                             loc);
  }

  "{c}"   return TOK(COMPLEMENT);
  "{T}"   return TOK(TRANSPOSITION);

  /* Special constructs.  */
  "(?@"   context.clear(); yy_push_state(SC_CONTEXT);
  "(?#"[^)]*")"  continue;

  /* Weights. */
  "<"     yy_push_state(SC_WEIGHT);

  /* Labels.  */
  {char}        return parser::make_LETTER(yytext, loc);
  "'"[^\']+"'"  return parser::make_LETTER(std::string(yytext+1, yyleng-2), loc);

  /* Character classes.  */
  "["     yy_push_state(SC_CLASS); return parser::make_LBRACKET(loc);

  \\.|.|\n   driver_.invalid(loc, yytext);
}

<SC_CLASS>{ /* Character-class.  Initial [ is eaten. */
  "]" {
    BEGIN INITIAL;
    return parser::make_RBRACKET(loc);
  }
  "-" return parser::make_DASH(loc);

  {char}        return parser::make_LETTER(yytext, loc);
  "'"[^\']+"'"  return parser::make_LETTER(std::string(yytext+1, yyleng-2), loc);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a character-class");
    BEGIN INITIAL;
    return parser::make_RBRACKET(loc);
  }
}

<SC_CONTEXT>{ /* Context embedded in a $(?@...) directive.  */
  "("   {
    ++nesting;
    context += yytext;
  }
  ")"   {
    if (nesting)
      {
        --nesting;
        context += yytext;
      }
    else
      {
        yy_pop_state();
        driver_.context(context);
        context.clear();
      }
  }
  [^()]+   context += yytext;
}

<SC_WEIGHT>{ /* Weight.  */
  "<"                           {
    ++nesting;
    s += yytext;
  }

  ">"                           {
    if (nesting)
      {
        --nesting;
        s += yytext;
      }
    else
      {
        yy_pop_state();
        return parser::make_WEIGHT(s, loc);
      }
  }
  [^<>]+       s += yytext;
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
    if (s.empty())
      return -1;
    else
      return lexical_cast<int>(d, loc, s);
  }

  /// Decode a quantifier's value: "1,2" etc.
  ///
  /// We used to include the braces in \a, but a libc++ bug in
  /// regex made the following regex unportable.
  /// http://llvm.org/bugs/show_bug.cgi?id=16135
  irange_type
  quantifier(driver& d, const location& loc, const std::string& s)
  {
    std::regex arity_re{"([0-9]*)(,?)([0-9]*)",
        std::regex::extended};
    std::smatch minmax;
    if (!std::regex_match(s, minmax, arity_re))
      throw std::runtime_error("cannot match arity: " + s);
    irange_type res{arity(d, loc, minmax[1].str()),
        arity(d, loc, minmax[3].str())};
    if (minmax[2].str().empty())
      // No comma: single argument.
      std::get<1>(res) = std::get<0>(res);
    return res;
  }
}

// Do not use %option noyywrap, because then flex generates the
// same definition of yywrap, but outside the namespaces, so it
// defines it for ::yyFlexLexer instead of
// ::vcsn::rat::yyFlexLexer.
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
