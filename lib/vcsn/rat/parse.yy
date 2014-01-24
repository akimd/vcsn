// -*- mode: c++ -*-

%require "3.0"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define api.namespace {vcsn::rat}
%define api.value.type variant
%define api.token.constructor

%code requires
{
  #include <iostream>
  #include <list>
  #include <set>
  #include <string>
  #include <tuple>
  #include "location.hh"
  #include <vcsn/core/rat/ratexp.hh>
  #include <lib/vcsn/rat/fwd.hh>

  namespace vcsn
  {
    namespace rat
    {
      struct braced_ratexp
      {
        exp_t exp;
        bool parens = false;
        braced_ratexp& operator=(exp_t e)
        {
          exp = e;
          return *this;
        }
      };

      using irange_type = std::tuple<int, int>;
    }
  }
}

%code provides
{
  #define YY_DECL_(Class) \
    parser::symbol_type Class lex(driver& driver_)
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
      /// Generate a ratexp for "e{range.first, range.second}".
      static
      exp_t power(const dyn::ratexpset& rs, exp_t e, std::tuple<int, int> range);
      /// Generate a ratexp matching one character amongst \a chars.
      static
      exp_t char_class(const dyn::ratexpset& rs, const std::set<char>& chars);

      /// Use our local scanner object.
      static
      inline
      parser::symbol_type
      yylex(driver& driver_)
      {
        return driver_.scanner_->lex(driver_);
      }
    }
  }
}

%param { driver& driver_ }

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

%printer { yyo << '"' << $$ << '"'; } <std::string>;
%printer { yyo << '['; for (auto c: $$) yyo << c; yyo << ']'; }
         <std::set<char>>;
%printer { yyo << '<' << $$ << '>'; } "weight";
%printer { driver_.ratexpset_->print(yyo, $$.exp); } <braced_ratexp>;

%token
  AMPERSAND  "&"
  COLON      ":"
  COMMA      ","
  COMPLEMENT "{c}"
  DOT        "."
  END 0      "end"
  LPAREN     "("
  ONE        "\\e"
  PERCENT    "%"
  PLUS       "+"
  RPAREN     ")"
  ZERO       "\\z"
;

%token <irange_type> STAR "*";
%token <std::string> LETTER  "letter";
%token <std::string> WEIGHT  "weight";
%token <std::set<char>> CLASS "character-class";

%type <braced_ratexp> exp exps weights;

%precedence RWEIGHT
%left "+"
%left ":" "%"
%left "&"
%left "."
%right "weight" // Match longest series of "weight".
%precedence LWEIGHT   // weights exp . "weight": reduce for the LWEIGHT rule.
%precedence "(" "\\z" "\\e" "letter" "character-class"
%precedence CONCAT
%precedence "*" "{c}"

%start exps
%%

exps:
  // Provide a value for $$ only for sake of traces: shows the result.
  exp  { driver_.result_ = ($$ = $1).exp; }
;

exp:
  exp "." exp                 { $$ = MAKE(mul, $1.exp, $3.exp); }
| exp "&" exp                 { $$ = MAKE(intersection, $1.exp, $3.exp); }
| exp ":" exp                 { $$ = MAKE(shuffle, $1.exp, $3.exp); }
| exp "+" exp                 { $$ = MAKE(add, $1.exp, $3.exp); }
| exp "%" exp                 { $$ = MAKE(intersection,
                                          $1.exp, MAKE(complement, $3.exp)); }
| weights exp %prec LWEIGHT   { $$ = MAKE(mul, $1.exp, $2.exp); }
| exp weights %prec RWEIGHT   { $$ = MAKE(mul, $1.exp, $2.exp); }
| exp exp %prec CONCAT
  {
    // See README.txt.
    if (!$1.parens && !$2.parens)
      $$ = MAKE(concat, $1.exp, $2.exp);
    else
      {
        $$.exp = MAKE(mul, $1.exp, $2.exp);
        $$.parens = $2.parens;
      }
  }
| exp "*"          { $$ = power(driver_.ratexpset_, $1.exp, $2); }
| exp "{c}"        { $$ = MAKE(complement, $1.exp); }
| ZERO             { $$ = MAKE(zero); }
| ONE              { $$ = MAKE(one); }
| "letter"         { TRY(@$, $$ = MAKE(atom, $1)); }
| "character-class" { $$ = char_class(driver_.ratexpset_, $1); }
| "(" exp ")"      { $$.exp = $2.exp; $$.parens = true; }
;

weights:
  "weight"         { TRY(@$ + 1, $$ = MAKE(lmul, $1, MAKE(one))); }
| "weight" weights { TRY(@$ + 1, $$ = MAKE(lmul, $1, $2.exp)); }
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
            res = es->mul(power(es, e, min, min), res);
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
                res = es->mul(res, e);
            }
          if (min < max)
            {
              exp_t sum = es->one();
              for (int n = 1; n <= max - min; ++n)
                sum = es->add(sum, power(es, e, n, n));
              res = es->mul(res, sum);
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

    static
    exp_t
    char_class(const dyn::ratexpset& rs, const std::set<char>& chars)
    {
      exp_t res = rs->zero();
      for (auto c: chars)
        res = rs->add(res, rs->atom(std::string(1, c)));
      return res;
    }

    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
