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

%x SC_COMMENT SC_STRING SC_XML

alpha   [a-zA-Z\200-\377_]
digit   [0-9]
IDENT   {alpha}({alpha}|{digit})*
NUM     [-]?("."{digit}+|{digit}+("."{digit}*)?)
ID      {IDENT}|{NUM}

%%
%{
  // Growing string, for SC_STRING/SC_XML.
  auto s = std::string{};
  // Level of nesting of "<"/">" for SC_XML.
  int nesting = 0;
  loc.step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "digraph"  return TOK(DIGRAPH);
  "edge"     return TOK(EDGE);
  "graph"    return TOK(GRAPH);
  "node"     return TOK(NODE);
  "subgraph" return TOK(SUBGRAPH);
  "{"        return TOK(LBRACE);
  "}"        return TOK(RBRACE);
  "["        return TOK(LBRACKET);
  "]"        return TOK(RBRACKET);
  "="        return TOK(EQ);
  "->"       return TOK(ARROW);
  ":"        return TOK(COLON);
  ","        return TOK(COMMA);
  ";"        return TOK(SEMI);

  "//".*     loc.step(); continue;
  "/*"       BEGIN(SC_COMMENT);
  "\""       BEGIN(SC_STRING);
  "<"        ++nesting; BEGIN(SC_XML);
  {ID}       {
               return parser::make_ID
                 (string_t{std::string{yytext, size_t(yyleng)}},
                  loc);
             }
  [ \t]+     loc.step(); continue;
  \n+        LINE(yyleng); loc.step(); continue;
  .          driver_.error(loc, std::string{"invalid character: "}+yytext);
}

<SC_COMMENT>{
  [^*\n]*        loc.step(); continue;
  "*"+[^*/\n]*   loc.step(); continue;
  "\n"+          LINE(yyleng); loc.step(); continue;
  "*"+"/"        BEGIN(INITIAL);
}

<SC_STRING>{ /* Handling of the strings.  Initial " is eaten. */
  \" {
    BEGIN(INITIAL);
    return parser::make_ID(string_t{s}, loc);
  }

  "\\".       s.push_back(yytext[1]);
  [^\\""\n]+  s.append(yytext, yyleng);
  "\n"+       LINE(yyleng); s.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a string");
    BEGIN(INITIAL);
    return parser::make_ID(string_t{s}, loc);
  }
}

<SC_XML>{ /* Handling of the XML/HTML strings.  Initial < is eaten. */
  ">" {
    if (--nesting == 0)
      {
        BEGIN(INITIAL);
        return parser::make_ID(string_t{s}, loc);
      }
    else
      s.push_back('>');
  }

  "<"       ++nesting; s.push_back('<');
  [^<>\n]+  s.append(yytext, yyleng);
  "\n"+     LINE(yyleng); s.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(loc, "unexpected end of file in a XML string");
    BEGIN(INITIAL);
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
