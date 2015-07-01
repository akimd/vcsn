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
  #include <set>
  #include <string>
  #include <tuple>
  #include "location.hh"
  #include <vcsn/core/rat/expression.hh>
  #include <vcsn/dyn/expression.hh>
  #include <lib/vcsn/rat/fwd.hh>

  namespace vcsn
  {
    namespace rat
    {
      /// An expression that "remembers" whether it was in
      /// parentheses.
      ///
      /// Used in LAW to distinguish "(a)(b)", "(a)b", "a(b)" from
      /// "ab": only in the last case the letters must be concatenated
      /// into a word.
      struct braced_expression
      {
        /// The expression parsed so far.
        dyn::expression exp;
        /// Whether there was a left-paren.
        bool lparen = false;
        /// Whether there was a right-paren.
        bool rparen = false;
        braced_expression& operator=(dyn::expression e)
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
  #include <lib/vcsn/rat/driver.hh>
  #include <lib/vcsn/rat/scan.hh>
  #include <vcsn/dyn/algos.hh>

  namespace vcsn
  {
    namespace rat
    {
      /// Generate an expression for "e <+ f = e % f + f".
      static inline
      dyn::expression prefer(const dyn::expression& e,
                             const dyn::expression& f);

      /// Get the context of the driver.
      static inline
      dyn::context ctx(const driver& d)
      {
        return d.context();
      }

      /// Get the identities of the driver.
      static inline
      identities ids(const driver& d)
      {
        return d.identities();
      }

      /// Use our local scanner object.
      static inline
      parser::symbol_type yylex(driver& d)
      {
        return d.scanner_->lex(d);
      }
    }
  }
}

%code top
{
  unsigned tape = 0;
}

%param { driver& driver_ }

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
%printer
{
  dyn::print($$.exp, yyo);
  yyo << ($$.lparen ? " (lpar, " : " (no lpar, ");
  yyo << ($$.rparen ? "rpar)" : "no rpar)");
} <braced_expression>;
%printer { dyn::print($$, yyo); } <dyn::weight>;

%token
  AMPERSAND  "&"
  BACKSLASH  "{\\}"
  CARET      "^"
  COLON      ":"
  COMMA      ","
  COMPLEMENT "{c}"
  DASH       "-"
  DOT        "."
  END 0      "end"
  LBRACKET   "["
  LPAREN     "("
  LT_PLUS    "<+"
  ONE        "\\e"
  PERCENT    "%"
  PIPE       "|"
  PLUS       "+"
  RBRACKET   "]"
  RPAREN     ")"
  SLASH      "{/}"
  TRANSPOSITION "{T}"
  ZERO       "\\z"
;

%token <irange_type> STAR "*";
%token <std::string> LETTER "letter";
%token <std::string> WEIGHT "weight";

%type <braced_expression> exp input;
%type <dyn::weight> weights;
%type <std::set<std::pair<std::string,std::string>>> class;

%left "|"
%left "+" "<+"
%left "%"
%left "&" ":"
%left "{/}"
%right "{\\}"
%left "."
%precedence CONCAT // exp exp . "(": reduce
%right "weight" // Match longest series of "weight".
%precedence RWEIGHT
%precedence "letter" "\\z" "\\e" "[" "(" // RWEIGHT < LETTER: a <x> . b => shift
%precedence LWEIGHT  // weights exp . "weight": reduce for the LWEIGHT rule.
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
  exp "." exp                 { $$ = dyn::multiply($1.exp, $3.exp); }
| exp "&" exp                 { $$ = dyn::conjunction($1.exp, $3.exp); }
| exp ":" exp                 { $$ = dyn::shuffle($1.exp, $3.exp); }
| exp "+" exp                 { $$ = dyn::sum($1.exp, $3.exp); }
| exp "<+" exp                { $$ = prefer($1.exp, $3.exp); }
| exp "{\\}" exp              { $$ = dyn::ldiv($1.exp, $3.exp); }
| exp "{/}" exp               { $$ = dyn::rdiv($1.exp, $3.exp); }
| exp "%" exp                 { $$ = dyn::conjunction($1.exp,
                                                      dyn::complement($3.exp)); }
| exp "|" { ++driver_.tape_; } exp { $$ = dyn::tuple($1.exp, $4.exp); }
| weights exp %prec LWEIGHT   { $$ = dyn::left_mult($1, $2.exp); }
| exp weights %prec RWEIGHT   { $$ = dyn::right_mult($1.exp, $2); }
| exp exp %prec CONCAT
  {
    // See README.txt.
    if (!$1.rparen && !$2.lparen)
      $$ = dyn::concatenate($1.exp, $2.exp);
    else
      {
        $$.exp = dyn::multiply($1.exp, $2.exp);
        $$.lparen = $1.lparen;
        $$.rparen = $2.rparen;
      }
  }
| exp "*"           { $$ = dyn::multiply($1.exp,
                                         std::get<0>($2), std::get<1>($2)); }
| exp "{c}"         { $$ = dyn::complement($1.exp); }
| exp "{T}"         { $$ = dyn::transposition($1.exp); }
| "\\z"             { $$ = dyn::expression_zero(ctx(driver_), ids(driver_)); }
| "\\e"             { $$ = dyn::expression_one(ctx(driver_), ids(driver_)); }
| LETTER            { $$ = driver_.make_atom(@1, $1); }
| "[" class "]"     { $$ = dyn::to_expression(ctx(driver_), ids(driver_),
                                              $2, true); }
| "[" "^" class "]" { $$ = dyn::to_expression(ctx(driver_), ids(driver_),
                                              $3, false); }
| "(" { tape = driver_.tape_; } exp ")"
                    {
                      driver_.tape_ = tape;
                      $$.exp = $3.exp;
                      $$.lparen = $$.rparen = true;
                    }
;

weights:
  "weight"         { $$ = driver_.make_weight(@1, $1); }
| "weight" weights { $$ = dyn::multiply(driver_.make_weight(@1, $1), $2); }
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
    // "e <+ f = e + (f % e) = e + e{c} & f".
    static
    dyn::expression prefer(const dyn::expression& e, const dyn::expression& f)
    {
      return dyn::sum(e, dyn::conjunction(dyn::complement(e), f));
    }

    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
