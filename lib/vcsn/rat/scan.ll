%option c++
%option prefix="rat" outfile="lex.yy.c"
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
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
# pragma GCC diagnostic ignored "-Wsuggest-attribute=pure"
#endif
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
    yylloc->end.column = 1;                     \
    yylloc->lines(Line);                        \
 } while (false)

#define YY_USER_ACTION                          \
  yylloc->columns(yyleng);

#define TOK(Token)                              \
  vcsn::rat::parser::token::Token

  namespace vcsn
  {
    namespace rat
    {
      namespace
      {
        using irange_type = sem_type::irange_type;
        irange_type
        quantifier(driver& d, const location& loc, const std::string& s);
      }

      // Do not use %option noyywrap, because then flex generates the
      // same definition of yywrap, but outside the namespaces, so it
      // defines it for ::yyFlexLexer instead of
      // ::vcsn::rat::yyFlexLexer.
      int yyFlexLexer::yywrap() { return 1; }

#define ratalloc myratalloc
void *myratalloc (yy_size_t  );
#define ratrealloc myratrealloc
void *myratrealloc (void *,yy_size_t  );
#define ratfree myratfree
void myratfree (void *  );

      //    }
      //  }
%}

%x SC_CONTEXT SC_WEIGHT

char      ([a-zA-Z0-9_]|\\[<>{}()+.*:\"])

%%
%{
  // Count the number of opened braces in SC_WEIGHT, and parens in SC_CONTEXT.
  unsigned int nesting = 0;
  // Build a context string.  "static" only to save build/dtor.
  static std::string context;

  yylloc->step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "("     yylval->ival = 0; return TOK(LPAREN);
  ")"     yylval->ival = 0; return TOK(RPAREN);

  "["     yylval->ival = 1; return TOK(LPAREN);
  "]"     yylval->ival = 1; return TOK(RPAREN);
  "+"     return TOK(SUM);
  "."     return TOK(DOT);
  ","     return TOK(COMMA);
  "\\e"   return TOK(ONE);
  "\\z"   return TOK(ZERO);

  /* Quantifiers.  */
  "*"|"{*}"              {
      yylval->irange = std::make_tuple(-1, -1);
      return TOK(STAR);
  }
  "?"|"{?}"              {
      yylval->irange = std::make_tuple(0, 1);
      return TOK(STAR);
  }
  "{+}"                  {
      yylval->irange = std::make_tuple(1, -1);
      return TOK(STAR);
  }
  "{"[0-9]*,?[0-9]*"}"    {
      yylval->irange
        = quantifier(driver_, *yylloc,
                     {yytext+1, static_cast<size_t>(yyleng)-2});
      return TOK(STAR);
  }


  "(?@"   context.clear(); yy_push_state(SC_CONTEXT);
  "(?#"[^)]*")"  continue;

  "<"     yylval->sval = new std::string(); yy_push_state(SC_WEIGHT);

  {char}  yylval->sval = new std::string(yytext); return TOK(LETTER);

  "\n"    continue;

  \\.|.   driver_.invalid(*yylloc, yytext);
}

<SC_WEIGHT>{ /* Weight with Vcsn Syntax*/
  "<"                           {
    ++nesting;
    *yylval->sval += yytext;
  }

  ">"                           {
    if (nesting)
      {
        --nesting;
        *yylval->sval += yytext;
      }
    else
      {
        yy_pop_state();
        return TOK(WEIGHT);
      }
  }
  [^<>]+       *yylval->sval += yytext;
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
%%

   //namespace vcsn
   //{
   //  namespace rat
   //  {

    namespace
    {
      // Safe conversion to a numeric value.
      // The name parser_impl_ is chosen so that SCAN_ERROR can be used
      // from out of the scanner.
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

      // The value of s, a decimal number, or -1 if empty.
      int arity(driver& d, const location& loc, const std::string& s)
      {
        if (s.empty())
          return -1;
        else
          return lexical_cast<int>(d, loc, s);
      }

      // Decode a quantifier's value: "1,2" etc.
      //
      // We used to include the braces in \a, but a libc++ bug in
      // regex made the following regex unportable.
      // http://llvm.org/bugs/show_bug.cgi?id=16135
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

    void ratFlexLexer::scan_open_(std::istream& f)
    {
      set_debug(!!getenv("YYSCAN"));
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(yy_create_buffer(&f, YY_BUF_SIZE));
    }

    void ratFlexLexer::scan_close_()
    {
      yypop_buffer_state();
    }
  }
}

// Local Variables:
// mode: C++
// End:
