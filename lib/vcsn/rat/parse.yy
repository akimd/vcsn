// -*- mode: c++ -*-

%require "3.0"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define api.namespace {vcsn::rat}

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include <tuple>
  #include "location.hh"
  #include <vcsn/core/rat/ratexp.hh>
  #include <lib/vcsn/rat/fwd.hh>

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
        using irange_type = std::tuple<int, int>;
        irange_type irange;
        bool parens = false;
        // These guys _can_ be put into a union.
        union
        {
          int ival;
          std::string* sval;
        };
      };
    }
  }
  #define YYSTYPE vcsn::rat::sem_type
}

%code provides
{
  #define YY_DECL_(Class)                               \
    int Class lex(parser::semantic_type* yylval,        \
                  parser::location_type* yylloc,        \
                  driver& driver_)
  #define YY_DECL YY_DECL_(yyFlexLexer::)
}

%code
{
  #include <vcsn/dyn/ratexpset.hh>
  #include <lib/vcsn/rat/driver.hh>
  #include <lib/vcsn/rat/scan.hh>

  namespace vcsn
  {
    namespace rat
    {
      static
      exp_t power(const dyn::ratexpset& rs, exp_t e, int min, int max);

      static
      exp_t power(const dyn::ratexpset& rs, exp_t e, std::tuple<int, int> range);

      /// Use our local scanner object.
      static
      inline
      int
      yylex(parser::semantic_type* yylval,
            parser::location_type* yylloc,
            driver& driver_)
      {
        return driver_.scanner_->lex(yylval, yylloc, driver_);
      }
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

%printer { debug_stream() << $$; } <ival>;
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
%printer { debug_stream() << '<' << *$$ << '>'; } "weight";
%printer { driver_.ratexpset_->print(debug_stream(), $$); } <node>;
%destructor { delete $$; } <sval>;

%token <ival>   LPAREN  "("
                RPAREN  ")"

%token  SUM   "+"
        AMPERSAND "&"
        COMPLEMENT "{c}"
        COLON ":"
        PERCENT "%"
        DOT   "."
        ONE   "\\e"
        ZERO  "\\z"
        COMMA  ","
;

%token <irange> STAR "*";
%token <sval> LETTER  "letter";
%token <sval> WEIGHT  "weight";

%type <node> exp exps weights;

%precedence RWEIGHT
%left "+"
%left ":" "%"
%left "&"
%left "."
%right "weight" // Match longest series of "weight".
%precedence LWEIGHT   // weights exp . "weight": reduce for the LWEIGHT rule.
%precedence "(" "\\z" "\\e" "letter"
%precedence CONCAT
%precedence "*" "{c}"

%start exps
%%

exps:
  // Provide a value for $$ only for sake of traces: shows the result.
  exp  { $$ = driver_.result_ = $1; }
;

exp:
  exp "." exp                 { $$ = MAKE(mul, $1, $3); }
| exp "&" exp                 { $$ = MAKE(intersection, $1, $3); }
| exp ":" exp                 { $$ = MAKE(shuffle, $1, $3); }
| exp "+" exp                 { $$ = MAKE(add, $1, $3); }
| exp "%" exp                 { $$ = MAKE(intersection,
                                          $1, MAKE(complement, $3)); }
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
| exp "*"          { $$ = power(driver_.ratexpset_, $1, $2); }
| exp "{c}"        { $$ = MAKE(complement, $1); }
| ZERO             { $$ = MAKE(zero); }
| ONE              { $$ = MAKE(one); }
| LETTER           { TRY(@$, $$ = MAKE(atom, *$1)); delete $1; }
| "(" exp ")"      { assert($1 == $3); $$ = $2; $<parens>$ = true; }
;

weights:
  "weight"         { TRY(@$ + 1, $$ = MAKE(rmul, MAKE(one), *$1)); delete $1;}
| "weight" weights { TRY(@$ + 1, $$ = MAKE(lmul, *$1, $2)); delete $1; }
;

%%

namespace vcsn
{
  namespace rat
  {
    static
    exp_t
    power(const dyn::ratexpset& es, exp_t e, int min, int max)
    {
      exp_t res;
      if (max == -1)
        {
          res = es->star(e);
          if (min != -1)
            res = es->concat(power(es, e, min, min), res);
        }
      else
        {
          if (min == -1)
            min = 0;
          if (min == 0)
            res = es->one();
          else
            {
              res = e;
              for (int n = 1; n < min; ++n)
                res = es->concat(res, e);
            }
          if (min < max)
            {
              exp_t sum = es->one();
              for (int n = 1; n <= max - min; ++n)
                sum = es->add(sum, power(es, e, n, n));
              res = es->concat(res, sum);
            }
        }
      return res;
    }

    static
    exp_t
    power(const dyn::ratexpset& es, exp_t e, std::tuple<int, int> range)
    {
      return power(es, e, std::get<0>(range), std::get<1>(range));
    }

    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
