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

#define MAKE(Kind, ...)                         \
      driver_.kratexps->Kind(__VA_ARGS__)
}

%initial-action
{
  @$ = driver_.location_;
}

%printer { debug_stream() << $$; } <ival>;
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
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

%token  <sval> ATOM    "atom"
               WEIGHT  "weight";

%type <node> exp term lexp factor leaf factors weights weights.opt;


%left "+"
%left "."

%start exps
%%

exps:
  exp  { driver_.result_ = $1; }
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
| factors factor                { $$ = MAKE(mul, $1, $2); }
;

factor:
  leaf                          { $$ = $1; }
| factor "*"                    { $$ = MAKE(star, $1); }
| "(" exp ")"                   { $$ = $2; assert($1 == $3); }
;

leaf:
  ZERO     { $$ = MAKE(zero); }
| ONE      { $$ = MAKE(unit); }
| ATOM     { try { $$ = MAKE(atom, *$1); }
             catch (std::exception& e) { error(@$, e.what()); YYERROR; }
             delete $1; }
;

weights.opt:
  /* empty */                   { $$ = MAKE(unit); }
| weights                       { $$ = $1; }
;

weights:
  "weight"
  {
    try { $$ = MAKE(weight, MAKE(unit), $1); }
    catch (std::exception& e) { error(@$ + 1, e.what()); YYERROR; }
  }
| "weight" weights
  {
    try { $$ = MAKE(weight, $1, $2); }
    catch (std::exception& e) { error(@1 + 1, e.what()); YYERROR; }
  }
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
