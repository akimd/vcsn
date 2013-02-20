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
  #include <vcsn/core/rat/ratexp.hh>
  #include <vcsn/core/rat/abstract_ratexpset.hh>
  #include <lib/vcsn/rat/driver.hh>

  namespace vcsn
  {
    namespace rat
    {
      // (Complex) objects such as shared_ptr cannot be put in a
      // union, even in C++11.  So cheat, and store a struct instead
      // of an union.  See README.txt.
      struct sem_type
      {
        exp_t node;
        // These guys _can_ be put into a union.
        union
        {
          int ival;
          std::string* sval;
          char cval;
        };
        bool parens = false;
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

%code top
{
  #include <cassert>

  /// Call the factory to make a Kind.
#define MAKE(Kind, ...)                         \
  driver_.ratexpset_->Kind(__VA_ARGS__)

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
%printer { driver_.ratexpset_->print(debug_stream(), $$); } <node>;
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

%type <node> exp exps weights;

%left RWEIGHT
%left "+"
%left "."
%right "weight" // Match longest series of "weight".
%left LWEIGHT   // weights exp . "weight": reduce for the LWEIGHT rule.
%left "(" "\\z" "\\e" "letter"
%left CONCAT
%right "*"

%start exps
%%

exps:
  // Provide a value for $$ only for sake of traces: shows the result.
  exp  { $$ = driver_.result_ = $1; }
;

exp:
  exp "." exp                 { $$ = MAKE(mul, $1, $3); }
| exp "+" exp                 { $$ = MAKE(add, $1, $3); }
| weights exp %prec LWEIGHT   { $$ = MAKE(mul, $1, $2); }
| exp weights %prec RWEIGHT   { $$ = MAKE(mul, $1, $2); }
| exp exp %prec CONCAT
  {
    // See README.txt.
    if (!$<parens>1 && !$<parens>2)
      $$ = MAKE(concat, $1, $2);
    else
      {
        $$ = MAKE(mul, $1, $2);
        $<parens>$ = $<parens>2;
      }
  }
| exp "*"                     { $$ = MAKE(star, $1); }
| ZERO                        { $$ = MAKE(zero); }
| ONE                         { $$ = MAKE(unit); }
| LETTER                      { TRY(@$, $$ = MAKE(atom, {$1})); }
| "(" exp ")"                 { assert($1 == $3); $$ = $2; $<parens>$ = true; }
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
