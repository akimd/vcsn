// -*- mode: c++ -*-

%require "3.1"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define api.namespace {vcsn::rat}
%define api.value.type variant
%define api.value.automove
%define api.location.include {<vcsn/misc/location.hh>}
%define api.token.constructor

%code requires
{
  #include <iostream>
  #include <tuple>
  #include <vcsn/core/rat/expression.hh>
  #include <vcsn/dyn/value.hh>
  #include <vcsn/misc/location.hh>
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
  do                                            \
    try                                         \
      {                                         \
        Stm;                                    \
      }                                         \
    catch (std::exception& e)                   \
      {                                         \
        error(Loc, e.what());                   \
        YYERROR;                                \
      }                                         \
  while (false)

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

%printer { yyo << '"' << $$ << '"'; } <std::string> <symbol>;
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
  AT              "@"
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
  LT_PLUS         "<+"
  ONE             "\\e"
  PERCENT         "%"
  PIPE            "|"
  PLUS            "+"
  RBRACKET        "]"
  RPAREN          ")"
  SLASH           "{/}"
  TRANSPOSITION   "{T}"
  TRANSPOSE       "{t}"
  ZERO            "\\z"
;

%token <irange_type> STAR "*";
%token <std::string> LETTER "letter";
%token <std::string> WEIGHT "weight";
%token <symbol>      LPAREN "(";

%type <braced_expression> exp input add tuple;
%type <std::vector<vcsn::dyn::expression>> tuple.1;
%type <dyn::weight> weights;
%type <class_t> class;

%left "+" "<+"
%left "@"
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
%precedence "*" "{c}" "{T}" "{t}"

%start input
%%

input:
  add terminator.opt
  {
    // Adjust with possible needed conversions e.g., `a*` -> `(a|a)*`.
    // Avoid it if possible, it is really expensive (see `vcsn score
    // -O 'b.expression'`).
    auto src = $1;
    TRY(@$, $$ = copy(src.exp, driver_.ctx_, driver_.ids_));
    if (0 < driver_.debug_level())
      std::cerr
        << "expression converted\n"
        << "  from: " << src.exp << " (" << context_of(src.exp) << ")\n"
        << "    to: " << $$.exp << " (" << context_of($$.exp) << ")\n";
    driver_.result_ = $$.exp;
    YYACCEPT;
  }
;

terminator.opt:
  %empty     {}
| ","        { driver_.scanner_->putback(','); }
| ")"        { driver_.scanner_->putback(')'); }
| "]"        { driver_.scanner_->putback(']'); }
;

add:
  tuple
| add "+" add     { $$ = dyn::add(std::move($1.exp), std::move($3.exp)); }
| add "@" add     { $$ = dyn::compose(std::move($1.exp), std::move($3.exp)); }
;

// Deal with `|`: a* | (b+c) | \e.
// Store in a vector and group the tuple into a single tuple.
tuple:
  { driver_.tape_push(); } tuple.1
  {
    auto tuple = $2;
    driver_.tape_pop();
    if (tuple.size() == 1)
      $$ = tuple.back();
    else if (tuple.size() == driver_.tape_ctx_.size())
      $$ = vcsn::dyn::tuple(tuple);
    else
      throw syntax_error(@$,
                         "not enough tapes: "
                         + std::to_string(tuple.size())
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
| exp "&:" exp                { $$ = dyn::infiltrate($1.exp, $3.exp); }
| exp "<+" exp                { $$ = prefer($1.exp, $3.exp); }
| exp "{\\}" exp              { $$ = dyn::ldivide($1.exp, $3.exp); }
| exp "{/}" exp               { $$ = dyn::rdivide($1.exp, $3.exp); }
| exp "%" exp                 { $$ = dyn::conjunction($1.exp,
                                                      dyn::complement($3.exp)); }
| weights exp %prec LWEIGHT   { $$ = dyn::lweight($1, $2.exp); }
| exp weights %prec RWEIGHT   { $$ = dyn::rweight($1.exp, $2); }
| exp exp %prec CONCAT
  {
    auto fst = $1;
    auto snd = $2;
    // See README.txt.
    if (!fst.rparen && !snd.lparen)
      $$ = dyn::concatenate(fst.exp, snd.exp);
    else
      {
        $$.exp = dyn::multiply(fst.exp, snd.exp);
        $$.lparen = fst.lparen;
        $$.rparen = snd.rparen;
      }
  }
| exp "*"
  { auto arity = $2;
    TRY(@$,
        $$ = dyn::multiply($1.exp,
                           std::get<0>(arity), std::get<1>(arity)));
  }
| exp "{c}"         { $$ = dyn::complement($1.exp); }
| "!" exp           { $$ = dyn::complement($2.exp); }
| exp "{T}"         { $$ = dyn::transposition($1.exp); }
| exp "{t}"         { $$ = dyn::transpose($1.exp); }
| "\\z"             { $$ = dyn::expression_zero(ctx(driver_), ids(driver_)); }
| "\\e"             { $$ = dyn::expression_one(ctx(driver_), ids(driver_)); }
| "letter"          { $$ = driver_.make_atom(@1, $1); }
| "[" class "]"     { $$ = driver_.make_expression(@$, $2, true); }
| "[" "^" class "]" { $$ = driver_.make_expression(@$, $3, false); }
| "(" add ")"
  {
    auto lparen = $1;
    $$.exp = $2.exp;
    $$.lparen = $$.rparen = true;
    if (!lparen.get().empty())
      $$ = dyn::name($$.exp, lparen.get());
  }
;

weights:
  "weight"         { $$ = driver_.make_weight(@1, $1); }
| "weight" weights { $$ = dyn::multiply(driver_.make_weight(@1, $1), $2); }
;

class:
  %empty                      {}
| class "letter"              { $$ = $1; auto l = $2; $$.emplace(l, l); }
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
      return dyn::add(e, dyn::conjunction(dyn::complement(e), f));
    }

    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
