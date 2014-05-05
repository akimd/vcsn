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
%printer
{
  yyo << '[';
  for (auto c: $$) yyo << c.first << "-" << c.second;
  yyo << ']';
} <std::set<std::pair<std::string,std::string>>>;
%printer { yyo << '<' << $$ << '>'; } "weight";
%printer { driver_.ratexpset_->print(yyo, $$.exp); } <braced_ratexp>;

%token
  AMPERSAND  "&"
  BACKSLASH  "{\\}"
  COLON      ":"
  COMMA      ","
  COMPLEMENT "{c}"
  DASH       "-"
  DOT        "."
  END 0      "end"
  LBRACKET   "["
  LPAREN     "("
  ONE        "\\e"
  PERCENT    "%"
  PLUS       "+"
  RBRACKET   "]"
  RPAREN     ")"
  TRANSPOSITION "{T}"
  SLASH      "{/}"
  ZERO       "\\z"
;

%token <irange_type> STAR "*";
%token <std::string> LETTER  "letter";
%token <std::string> WEIGHT  "weight";

%type <braced_ratexp> exp input weights;
%type <std::set<std::pair<std::string,std::string>>> class;

%left "+"
%left ":" "%"
%left "&"
%left "{/}"
%right "{\\}"
%left "."
%right "weight" // Match longest series of "weight".
%precedence LWEIGHT  // weights exp . "weight": reduce for the LWEIGHT rule.
%precedence RWEIGHT
%precedence "(" "\\z" "\\e" "letter" "["
%precedence CONCAT
%precedence "*" "{c}" "{T}"

%start input
%%

input:
  // Provide a value for $$ only for sake of traces: shows the result.
  exp               {
                      driver_.result_ = $1.exp;
                      $$.exp = $1.exp;
                    }
| exp terminator    {
                      driver_.result_ = $1.exp;
                      $$.exp = $1.exp;
                      YYACCEPT;
                    }
;

terminator:
  ","               { driver_.scanner_->yyin->putback(','); }
| ")"               { driver_.scanner_->yyin->putback(')'); }
;

exp:
  exp "." exp                 { $$ = MAKE(mul, $1.exp, $3.exp); }
| exp "&" exp                 { $$ = MAKE(conjunction, $1.exp, $3.exp); }
| exp ":" exp                 { $$ = MAKE(shuffle, $1.exp, $3.exp); }
| exp "+" exp                 { $$ = MAKE(add, $1.exp, $3.exp); }
| exp "{\\}" exp              { $$ = MAKE(ldiv, $1.exp, $3.exp); }
| exp "{/}" exp               { $$ = MAKE(rdiv, $1.exp, $3.exp); }
| exp "%" exp                 { $$ = MAKE(conjunction,
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
| exp "{T}"        { $$ = MAKE(transposition, $1.exp); }
| "\\z"            { $$ = MAKE(zero); }
| "\\e"            { $$ = MAKE(one); }
| LETTER           { TRY(@$, $$ = MAKE(atom, $1)); }
| "[" class "]"    { $$ = MAKE(char_class, $2); }
| "(" exp ")"      { $$.exp = $2.exp; $$.parens = true; }
;

weights:
  "weight"         { TRY(@$ + 1, $$ = MAKE(lmul, $1, MAKE(one))); }
| "weight" weights { TRY(@$ + 1, $$ = MAKE(lmul, $1, $2.exp)); }
;

class:
  %empty                    {}
| class LETTER              { $$ = $1; $$.emplace($2, $2); }
| class LETTER "-" LETTER   { $$ = $1; $$.emplace($2, $4); }
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

    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
