/* See <http://www.graphviz.org/content/dot-language>. */

%option nounput noyywrap
%option debug
%option prefix="dot" outfile="lex.yy.c"

%top{
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
}

%{
#include <string>
#include <iostream>
#include <lib/vcsn/dot/parse.hh>

#define LINE(Line)                              \
  do{                                           \
    yylloc->end.column = 1;                     \
    yylloc->lines(Line);                        \
 } while (false)

#define YY_USER_ACTION                          \
  yylloc->columns(yyleng);

#define TOK(Token)                              \
  vcsn::dot::parser::token::Token
%}

%x SC_COMMENT SC_STRING

alpha   [a-zA-Z\200-\377]
digit   [0-9]
ID      {alpha}(_|{alpha}|{digit})*
NUM     [-]?("."{digit}+|{digit}+("."{digit}*)?)

%%
%{
  std::string s;
  yylloc->step();
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
  ","        return TOK(COMMA);
  ";"        return TOK(SEMI);

  "//".*     continue;
  "/*"       BEGIN SC_COMMENT;
  "\""       BEGIN SC_STRING;
  {ID}|{NUM} {
               yylval->string = std::string{yytext, size_t(yyleng)};
               return TOK(ID);
             }
  [ \t]+     continue;
  \n+        LINE(yyleng);
  .          driver_.error(*yylloc, std::string{"invalid character: "}+yytext);
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
    yylval->string = s;
    return TOK(ID);
  }

  \\\"      s += '"';
  \\.       s.append(yytext, yyleng);
  [^\\""]+  s.append(yytext, yyleng);

  <<EOF>> {
    driver_.error(*yylloc, "unexpected end of file in a string");
    BEGIN INITIAL;
    return TOK(ID);
  }
}

%%
namespace vcsn
{
  namespace dot
  {
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
    // in the document: save the old context, switch to the new one.

    void
    driver::scan_open_(FILE *f)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
    }

    void
    driver::scan_open_(const std::string& e)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yyin = 0;
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_scan_bytes(e.c_str(), e.size());
    }

    void
    driver::scan_close_()
    {
      yypop_buffer_state();
      //if (yyin)
      //fclose(yyin);
    }
  }
}

// Local Variables:
// mode: C++
// End:
