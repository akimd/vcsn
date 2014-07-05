/* See <http://www.graphviz.org/content/dot-language>. */
%option c++
%option debug
%option noinput nounput

%top{
// Define YY_DECL.
#include <lib/vcsn/dot/parse.hh>

#define yyterminate() return parser::make_END(loc)
}

%{
#include <string>
#include <iostream>

#define LINE(Line)                              \
  do {                                          \
    loc.end.column = 1;                         \
    loc.lines(Line);                            \
 } while (false)

#define YY_USER_ACTION                          \
  loc.columns(yyleng);

#define TOK(Token)                              \
  parser::make_ ## Token (loc)

YY_FLEX_NAMESPACE_BEGIN

%}

%x SC_COMMENT SC_STRING

alpha   [a-zA-Z\200-\377]
digit   [0-9]
ID      {alpha}(_|{alpha}|{digit})*
NUM     [-]?("."{digit}+|{digit}+("."{digit}*)?)

%%
%{
  std::string s;
  loc.step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "digraph"  return TOK(DIGRAPH);
  "edge"     return TOK(EDGE);
  "graph"    return TOK(GRAPH);
  "node"     return TOK(NODE);
  "{"        return TOK(LBRACE);
  "}"        return TOK(RBRACE);
  "["        return TOK(LBRACKET);
  "]"        return TOK(RBRACKET);
  "="        return TOK(EQ);
  "->"       return TOK(ARROW);
  ":"        return TOK(COLON);
  ","        return TOK(COMMA);
  ";"        return TOK(SEMI);

  "//".*     continue;
  "/*"       BEGIN SC_COMMENT;
  "\""       BEGIN SC_STRING;
  {ID}|{NUM} return parser::make_ID(string_t{std::string{yytext, size_t(yyleng)}}, loc);
  [ \t]+     continue;
  \n+        LINE(yyleng);
  .          driver_.error(loc, std::string{"invalid character: "}+yytext);
}

<SC_COMMENT>{
  [^*\n]*        continue;
  "*"+[^*/\n]*   continue;
  "\n"+          LINE(yyleng);
  "*"+"/"        BEGIN(INITIAL);
}

<SC_STRING>{ /* Handling of the strings.  Initial " is eaten. */
  \" {
    BEGIN INITIAL;
    return parser::make_ID(string_t{s}, loc);
  }

  \\.       s.push_back(yytext[1]);
  [^\\""]+  s.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a string");
    BEGIN INITIAL;
    return parser::make_ID(string_t{s}, loc);
  }
}

%%
// Do not use %option noyywrap, because then flex generates the
// same definition of yywrap, but outside the namespaces.
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
