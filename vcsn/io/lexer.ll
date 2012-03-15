%{                                                      /* -*- C++ -*-  */
#include <string>
#include <cassert>
#include <stack>
#include "io/parse-rat-exp.hh"

#define STEP()					\
 do {						\
   yylloc->begin.line   = yylloc->end.line;	\
   yylloc->begin.column = yylloc->end.column;	\
 } while (false)

#define COL(Col)				\
  yylloc->end.column += (Col)

#define LINE(Line)				\
  do{						\
    yylloc->end.column = 1;			\
    yylloc->end.line += (Line);                 \
 } while (false)

#define YY_USER_ACTION				\
  COL(yyleng);

#define TOK(Token)                              \
  vcsn::rat_exp::parser::token::Token
%}
%option noyywrap nounput stack debug


%x SC_WEIGHT SC_WORD

vcsn_character      ([a-zA-Z0-9_]|\\[{}()+.*:\"])


%%
%{
  // Count the number of opened braces.
  unsigned int nesting = 0;
  // Grow a string before returning it.
  std::string* sval = 0;
  STEP();
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
  {vcsn_character}              {
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
  {vcsn_character}              {
    *sval += yytext;
  }
  \"                            {
    yy_pop_state();
    yylval->sval = sval;
    return TOK(WORD);
  }
  \<{vcsn_character}*\>         { // FIXME: check
    *sval += yytext;
  }
  .                             exit(51);
}

%%
