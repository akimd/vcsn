%option noyywrap nounput stack debug

%{
#include <string>
#include <cassert>
#include <stack>
#include "io/parse-rat-exp.hh"

#define LINE(Line)				\
  do{						\
    yylloc->end.column = 1;			\
    yylloc->lines (Line);                       \
 } while (false)

#define YY_USER_ACTION				\
  yylloc->columns (yyleng);

#define TOK(Token)                              \
  vcsn::rat::parser::token::Token
%}
%x SC_WEIGHT SC_WORD

char      ([a-zA-Z0-9_]|\\[{}()+.*:\"])

%%
%{
  // Count the number of opened braces.
  unsigned int nesting = 0;
  // Grow a string before returning it.
  std::string* sval = 0;

  yylloc->step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "("                           yylval->ival = 0; return TOK(LPAREN);
  ")"                           yylval->ival = 0; return TOK(RPAREN);

  "["                           yylval->ival = 1; return TOK(LPAREN);
  "]"                           yylval->ival = 1; return TOK(RPAREN);
  "+"                           return TOK(PLUS);
  "."                           return TOK(DOT);
  "*"                           return TOK(STAR);
  "\\e"                         return TOK(ONE);
  "\\z"                         return TOK(ZERO);

  "{"                           {
    sval = new std::string();
    yy_push_state(SC_WEIGHT);
  }
  {char}                        {
    yylval->sval = new std::string(yytext);
    return TOK(WORD);
  }
  "\n"                          continue;
  .                             exit(51); // FIXME

}

<SC_WEIGHT>{ /* Weight with Vcsn Syntax*/
  "{"                           {
    ++nesting;
    *sval += yytext;
  } // push brace
  "}"                           {
    if (nesting)
      {
        --nesting;
        *sval += yytext;
      }
    else
      {
        yy_pop_state();
        yylval->sval = sval;
        return TOK(WEIGHT);
      }
  }
  [^{}]+       { *sval += yytext; }
}

<SC_WORD>{ /* Word with Vcsn Syntax*/
  {char}                        { *sval += yytext; }
  \"                            {
    yy_pop_state();
    yylval->sval = sval;
    return TOK(WORD);
  }
  \<{char}*\>         { // FIXME: check
    *sval += yytext;
  }
  .                             exit(51);
}

%%

// Local Variables:
// mode: C++
// End:
