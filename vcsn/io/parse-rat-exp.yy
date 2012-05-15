// -*- mode: c++ -*-
%require "2.5"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define namespace "vcsn::rat"
%name-prefix "vcsn::rat::"

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <vcsn/core/rat/node.hh>
  #include <vcsn/core/rat/abstract_kratexps.hh>
  #include <vcsn/io/driver.hh>

  namespace vcsn
  {
    namespace rat
    {
      struct sem_type
      {
        int ival;
        std::string* sval;
        char cval;
        driver::exp_t node;
      };
    }
  }
#define YYSTYPE vcsn::rat::sem_type
}

%code provides
{
  #define YY_DECL                                               \
    int                                                         \
    vcsn::rat::lex(vcsn::rat::parser::semantic_type* yylval,    \
                   vcsn::rat::parser::location_type* yylloc,    \
                   vcsn::rat::driver& driver_)

  namespace vcsn
  {
    namespace rat
    {
      int
      lex(parser::semantic_type* yylval,
          parser::location_type* yylloc,
          vcsn::rat::driver& driver_);
    }
  }
}

%parse-param { driver& driver_ }
%lex-param   { driver& driver_ }

%code
{
  #include <cassert>

  /// Call the factory to make a Kind.
#define MAKE(Kind, ...)                         \
      driver_.kratexps->Kind(__VA_ARGS__)

  /// Run Stm, and bounces exceptions into parse errors at Loc.
#define TRY(Loc, Stm)                           \
  try                                           \
    {                                           \
      Stm;                                      \
    }                                           \
  catch (std::exception& e)                     \
    {                                           \
      error(Loc, e.what());                     \
      YYERROR;                                  \
    }
}

%initial-action
{
  @$ = driver_.location_;
}

%printer { debug_stream() << $$; } <ival> <cval>;
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
%printer { debug_stream() << '{' << *$$ << '}'; } "weight";
%printer { driver_.kratexps->print(debug_stream(), $$); } <node>;
%destructor { delete $$; } <sval>;

%token <ival>   LPAREN  "("
                RPAREN  ")"
%token  SUM  "+"
        DOT   "."
        STAR  "*"
        ONE   "\\e"
        ZERO  "\\z"
;

%token <cval> LETTER  "letter";
%token <sval> WEIGHT  "weight";

%type <node> exp exps term lexp factor leaf factors weights weights.opt;


%left "+"
%left "."

%start exps
%%

exps:
  // Provide a value for $$ only for sake of traces: shows the result.
  exp  { $$ = driver_.result_ = $1; }
;

exp:
  term                          { $$ = $1; }
| exp "." exp                   { $$ = MAKE(mul, $1, $3); }
| exp "+" exp                   { $$ = MAKE(add, $1, $3); }
;

term:
  lexp weights.opt              { $$ = MAKE(mul, $1, $2); }
;

lexp:
  weights.opt factors           { $$ = MAKE(mul, $1, $2); }
| lexp weights factors          { $$ = MAKE(mul, $1, MAKE(mul, $2, $3)); }
;

factors:
  factor                        { $$ = $1; }
| factors factor                { $$ = MAKE(word, $1, $2); }
;

factor:
  leaf                          { $$ = $1; }
| factor "*"                    { $$ = MAKE(star, $1); }
;

leaf:
  ZERO     { $$ = MAKE(zero); }
| ONE      { $$ = MAKE(unit); }
| LETTER   { try { $$ = MAKE(atom, {$1}); }
             catch (std::exception& e) { error(@$, e.what()); YYERROR; } }
| "(" exp ")"                   { $$ = $2; assert($1 == $3); }
;

weights.opt:
  /* empty */                   { $$ = MAKE(unit); }
| weights                       { $$ = $1; }
;

weights:
  "weight"          { TRY(@$ + 1, $$ = MAKE(weight, MAKE(unit), $1)); }
| "weight" weights  { TRY(@$ + 1, $$ = MAKE(weight, $1, $2)); }
;

%%

namespace vcsn
{
  namespace rat
  {
    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
