%option noyywrap nounput stack debug

%{
#include <string>
#include <cassert>
#include <stack>
#include <iostream>
#include <vcsn/io/parse-rat-exp.hh>

#define LINE(Line)				\
  do{						\
    yylloc->end.column = 1;			\
    yylloc->lines(Line);                        \
 } while (false)

#define YY_USER_ACTION				\
  yylloc->columns(yyleng);

#define TOK(Token)                              \
  vcsn::rat::parser::token::Token
%}
%x SC_WEIGHT SC_ATOM

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

  "("     yylval->ival = 0; return TOK(LPAREN);
  ")"     yylval->ival = 0; return TOK(RPAREN);

  "["     yylval->ival = 1; return TOK(LPAREN);
  "]"     yylval->ival = 1; return TOK(RPAREN);
  "+"     return TOK(SUM);
  "."     return TOK(DOT);
  "*"     return TOK(STAR);
  "\\e"   return TOK(ONE);
  "\\z"   return TOK(ZERO);

  "{"     sval = new std::string(); yy_push_state(SC_WEIGHT);
  {char}  yylval->sval = new std::string(yytext); return TOK(ATOM);
  "\n"    continue;
  .       driver_.invalid(*yylloc, yytext);

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

<SC_ATOM>{ /* Word with Vcsn Syntax*/
  {char}        *sval += yytext;
  \"            yy_pop_state(); yylval->sval = sval; return TOK(ATOM);
  \<{char}*\>   *sval += yytext;  // FIXME: check
  .             driver_.invalid(*yylloc, yytext);
}

%%

namespace vcsn
{
  namespace rat
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
    scan_file(const std::string& f)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yyin = f == "-" ? stdin : fopen(f.c_str(), "r");
      if (!yyin)
        {
          std::cerr << f << ": cannot open: " << strerror(errno) << std::endl;
          exit(1);
        }
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
    }

    void
    scan_string(const std::string& e)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yyin = 0;
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_scan_bytes(e.c_str(), e.size());
    }

    void
    scan_close()
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
