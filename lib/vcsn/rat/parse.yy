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

      /// An integer range, for quantifiers/exponents.
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
  #include <vcsn/misc/stream.hh>
  #include <vcsn/dyn/context.hh>

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
} <class_t>;
%printer { yyo << '<' << $$ << '>'; } "weight";
%printer
{
  dyn::print($$.exp, yyo);
  yyo << ($$.lparen ? " (lpar, " : " (no lpar, ");
  yyo << ($$.rparen ? "rpar)" : "no rpar)");
} <braced_expression>;
%printer
{
  const char* sep = "[[";
  for (auto e: $$)
    {
      yyo << sep << e;
      sep = " || ";
    }
  yyo << "]]";
} <std::vector<vcsn::dyn::expression>>;

%printer { dyn::print($$, yyo); } <dyn::weight>;

%token
  AMPERSAND       "&"
  AMPERSAND_COLON "&:"
  BACKSLASH       "{\\}"
  BANG            "!"
  CARET           "^"
  COLON           ":"
  COMMA           ","
  COMPLEMENT      "{c}"
  DASH            "-"
  DOT             "."
  END 0           "end"
  LBRACKET        "["
  LPAREN          "("
  LT_PLUS         "<+"
  ONE             "\\e"
  PERCENT         "%"
  PIPE            "|"
  PLUS            "+"
  RBRACKET        "]"
  RPAREN          ")"
  SLASH           "{/}"
  TRANSPOSITION   "{T}"
  ZERO            "\\z"
;

%token <irange_type> STAR "*";
%token <std::string> LETTER "letter";
%token <std::string> WEIGHT "weight";

%type <braced_expression> exp input sum tuple;
%type <std::vector<vcsn::dyn::expression>> tuple.1;
%type <dyn::weight> weights;
%type <class_t> class;

%left "+" "<+"
 //%left "|"
%left "%"
%left "&" ":" "&:"
%left "{/}"
%right "{\\}"
%left "!"
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
  sum terminator.opt
  {
    auto dim_exp = dyn::num_tapes(dyn::context_of($1.exp));
    auto dim_ctx = dyn::num_tapes(driver_.ctx_);
    if (dim_exp != dim_ctx)
      // num_tapes returns 0 on non lat.  In this case, 1 is clearer.
      throw syntax_error(@$,
                         "not enough tapes: "
                         + std::to_string(std::max(size_t{1}, dim_exp))
                         + " expected "
                         + std::to_string(std::max(size_t{1}, dim_ctx)));
    // Provide a value for $$ only for sake of traces: shows the result.
    $$ = $1;
    driver_.result_ = $$.exp;
    YYACCEPT;
  }
;

terminator.opt:
  %empty     {}
| ","        { driver_.scanner_->putback(','); }
| ")"        { driver_.scanner_->putback(')'); }
;

sum:
  tuple           { $$ = $1; }
| sum "+" sum     { $$ = dyn::sum($1.exp, $3.exp); }
;

// Deal with `|`: a* | (b+c) | \e.
// Store in a vector and group the tuple into a single tuple.
tuple:
  { driver_.tape_push(); } tuple.1
  {
    driver_.tape_pop();
    if ($2.size() == 1)
      $$ = $2.back();
    else if ($2.size() == driver_.tape_ctx_.size())
      $$ = vcsn::dyn::tuple($2);
    else
      throw syntax_error(@$,
                         "not enough tapes: "
                         + std::to_string($2.size())
                         + " expected "
                         + std::to_string(driver_.tape_ctx_.size()));
  }
;

tuple.1:
  exp
  {
    $$.emplace_back($1.exp);
  }
| tuple.1 "|" { driver_.tape_inc(@2); } exp
  {
    $$ = $1;
    $$.emplace_back($4.exp);
  }
;

exp:
  exp "." exp                 { $$ = dyn::multiply($1.exp, $3.exp); }
| exp "&" exp                 { $$ = dyn::conjunction($1.exp, $3.exp); }
| exp ":" exp                 { $$ = dyn::shuffle($1.exp, $3.exp); }
| exp "&:" exp                { $$ = dyn::infiltration($1.exp, $3.exp); }
| exp "<+" exp                { $$ = prefer($1.exp, $3.exp); }
| exp "{\\}" exp              { $$ = dyn::ldiv($1.exp, $3.exp); }
| exp "{/}" exp               { $$ = dyn::rdiv($1.exp, $3.exp); }
| exp "%" exp                 { $$ = dyn::conjunction($1.exp,
                                                      dyn::complement($3.exp)); }
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
| exp "*"           { TRY(@$,
                          $$ =
                          dyn::multiply($1.exp,
                                        std::get<0>($2), std::get<1>($2))); }
| exp "{c}"         { $$ = dyn::complement($1.exp); }
| "!" exp           { $$ = dyn::complement($2.exp); }
| exp "{T}"         { $$ = dyn::transposition($1.exp); }
| "\\z"             { $$ = dyn::expression_zero(ctx(driver_), ids(driver_)); }
| "\\e"             { $$ = dyn::expression_one(ctx(driver_), ids(driver_)); }
| "letter"          { $$ = driver_.make_atom(@1, $1); }
| "[" class "]"     { $$ = driver_.make_expression(@$, $2, true); }
| "[" "^" class "]" { $$ = driver_.make_expression(@$, $3, false); }
| "(" sum ")"       { $$.exp = $2.exp; $$.lparen = $$.rparen = true; }
;

weights:
  "weight"         { $$ = driver_.make_weight(@1, $1); }
| "weight" weights { $$ = dyn::multiply(driver_.make_weight(@1, $1), $2); }
;

class:
  %empty                      {}
| class "letter"              { $$ = $1; $$.emplace($2, $2); }
| class "letter" "-" "letter" { $$ = $1; $$.emplace($2, $4); }
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
