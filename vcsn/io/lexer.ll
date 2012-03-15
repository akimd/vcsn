%{                                                      /* -*- C++ -*-  */
#include <string>
#include <cassert>
#include <stack>
#include "io/parse-rat-exp.hh"

static unsigned int weight_level = 0;
static std::string* sval = 0;

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
    assert (!sval);
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
    ++weight_level;
    *sval += yytext;
  } // push brace
  "}"                           {
    if (weight_level)
      {
        --weight_level;
        *sval += yytext;
      }
    else
      {
        yy_pop_state();
        yylval->sval = sval;
        sval = 0;
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
    sval = 0;
    return TOK(WORD);
  }
  \<{vcsn_character}*\>         { // FIXME: check
    *sval += yytext;
  }
  .                             exit(51);
}

%%
