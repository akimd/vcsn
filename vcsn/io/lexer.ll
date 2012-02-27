%{                                                      /* -*- C++ -*-  */
#include <string>
#include <cassert>
#include <stack>
#include "io/parse-rat-exp.hh"

static unsigned int brace_level = 0;
static unsigned int weight_level = 0;
static std::string* sval = 0;
std::stack<unsigned int> brace_context_level;
void switch_context(std::string context_name);
extern int errors;

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

#define TOK(Token)       \
  rat_exp_parse_::parser::token::Token
%}
%option noyywrap nounput stack debug


%x VCSN_WEIGHT VCSN_WORD

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
<<<<<<< HEAD
    assert (!sval);
    sval = new std::string();
=======
    weight_string = new std::string();
>>>>>>> 88834fde866f4fdee23adc0c00ae234391a07e11
    yy_push_state(VCSN_WEIGHT);
  } // lex vcsn weight language FIXME: delete or no
  {vcsn_character}              {
    yylval->sval = new std::string(yytext);
    return TOK(WORD);
    // yy_push_state(VCSN_WORD);
  }
  "\n"                          continue;
  .                             exit(51); // FIXME

}

<VCSN_WEIGHT>{ /* Weight with Vcsn Syntax*/

  "{"                           {
    ++weight_level;
    weight_level += '{';
  } // push brace
  "}"                           {
<<<<<<< HEAD
    if (0 == weight_level)
    {
      yy_pop_state();
      yylval->sval = sval;
      sval = 0;
=======
    if(0 == weight_level)
    {
      yy_pop_state();
      yylval->sval = weight_string;
>>>>>>> 88834fde866f4fdee23adc0c00ae234391a07e11
      return TOK(WEIGHT);
    }
    else
    {
      --weight_level;
      weight_level += '}';
    }
  } // pop brace or leace
  "("                           {
    ++brace_level;
    weight_level += '(';
  } // push parenthesis
  ")"                           {
    if(0 == brace_level)
    {
      assert(brace_level == 0);
    }
    else
    {
      --brace_level;
<<<<<<< HEAD
      sval += ')';
    }
  } // pop parenthesis
  ([^(){}]|\\[(){}])+       {
    *sval += yytext;
=======
      weight_string += ')';
    }
  } // pop parenthesis
  ([a-zA-Z0-9]|\\[(){}])+       {
    *weight_string += yytext;
>>>>>>> 88834fde866f4fdee23adc0c00ae234391a07e11
  }
}

<VCSN_WORD>{ /* Word with Vcsn Syntax*/
  {vcsn_character}              {
<<<<<<< HEAD
    *sval += yytext;
  }
  \"                            {
    yy_pop_state();
    yylval->sval = sval;
    sval = 0;
=======
    *string_acu += yytext;
  }
  \"                            {
    yy_pop_state();
    yylval->sval = string_acu;
>>>>>>> 88834fde866f4fdee23adc0c00ae234391a07e11
    return TOK(WORD);
  }
  \<{vcsn_character}*\>         { // FIXME: check
    // \\l\(([^)]|\\))*\)
<<<<<<< HEAD
    *sval += yytext;
=======
    *string_acu += yytext;
>>>>>>> 88834fde866f4fdee23adc0c00ae234391a07e11
  }
  .                             exit(51);
}

%%

void
switch_context(std::string context_name)
{
  if("vcsn" == context_name)
  {
    yy_push_state(INITIAL);
    brace_context_level.push(0);
  }
}
// "\\s{[a-zA-Z]*}{"             {
//   std::string str_yytext(yytext);
//   switch_context(std::string(str_yytext.begin() + 3, str_yytext.end() - 1));
// }
