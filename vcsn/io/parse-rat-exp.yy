// -*- mode: c++ -*-
%pure-parser
%require "2.5"
%language "C++"

%name-prefix "rat_exp_parse_"
%expect 0
%error-verbose
%defines
%debug

%locations

%code requires
{
  #include <list>
  #include <string>
  #include "location.hh"

  typedef std::string weight_type;
  typedef std::list<weight_type*> weights_type;

  union YYSTYPE
  {
    int ival;
    std::string* sval;
    weight_type* weight;
    weights_type* weights;
  };

  #define YY_DECL                                                       \
    int rat_exp_parse_lex(YYSTYPE* yylval,                              \
                          rat_exp_parse_::location* yylloc)
  YY_DECL;
}

%code
{
  #include <cassert>
  #include <sstream>
  #define STRING_(Out, In)                        \
    do {                                        \
      std::stringstream o;                      \
      o << In;                                  \
      Out = new std::string(o.str());           \
    } while (false)
  #define STRING(Out, In)                         \
    STRING_(Out, '(' << In << ')')

  #define MAKE(Kind, ...)                         \
    make_ ## Kind(__VA_ARGS__)

  static
  weights_type*
  make_weights(weight_type* w)
  {
    return new weights_type {w};
  }

  static
  weights_type*
  make_weights(weight_type* w, weights_type* ws)
  {
    ws->push_front(w);
    return ws;
  }

  std::ostream&
  operator<<(std::ostream& o, const weights_type& ws)
  {
    o << "{";
    bool first = true;
    for (weight_type* w: ws)
      {
        if (!first)
          o << ", ";
        first = false;
        o << *w;
      }
    o << "}";
    return o;
  }

  std::string*
  make_term(std::string *lexp, weights_type* ws)
  {
    assert(lexp);
    std::string* res = lexp;
    if (ws)
      {
        STRING(res, "r" << *ws << *lexp);
        delete lexp;
        delete ws;
      }
    return res;
  }

  std::string*
  make_term(weights_type* ws, std::string *term)
  {
    assert(term);
    std::string *res = term;
    if (ws)
      {
        STRING(res, "l" << *ws << *term);
        delete term;
        delete ws;
      }
    return res;
  }

  std::string*
  make_term(std::string* lexp, weights_type* ws, std::string *factors)
  {
    assert(lexp);
    assert(ws);
    assert(factors);
    std::string* res;
    STRING(res, *lexp << "#(l" << *ws << *factors << ")");
    delete lexp;
    delete ws;
    delete factors;
    return res;
  }

}

%printer { debug_stream() << *$$; } <sval>;
%printer { debug_stream() << $$; } <ival>;

%destructor { delete $$; } <sval>;

%token <ival>   LPAREN "("
                RPAREN ")"

%token  PLUS "+"
        DOT  "."
        STAR "*"
        ONE  "\\e"
        ZERO "\\z"
;

%token  <sval> WORD    "word";
%token <weight> WEIGHT  "weight"

%type <sval> exp factor factors lexp term word;
%type <weights> weights weights.opt

%left "+"
%left "."

%start exps
%%

exps:
  exp { std::cout << *$1 << std::endl; }
;

exp:
  term
| exp "." exp  { STRING($$, *$1 << '.' << *$3); }
| exp "+" exp  { STRING($$, *$1 << '+' << *$3); }
;

term:
  lexp weights.opt { $$ = MAKE(term, $1, $2); }
;

lexp:
  weights.opt factors  { $$ = MAKE(term, $1, $2); }
| lexp weights factors { $$ = MAKE(term, $1, $2, $3); }
;

factors:
  factor
| factors factor  { STRING($$, *$1 << '#' << *$2); }
;

factor:
  word
| factor "*"   { STRING($$, *$1 << '*'); }
| "(" exp ")"  { $$ = $2; assert($1 == $3); }
;

word:
  ZERO  { STRING($$, "\\z"); }
| ONE   { STRING($$, "\\e"); }
| WORD  { STRING($$, *$1); }
;

weights.opt:
  /* empty */ { $$ = nullptr; }
| weights
;

weights:
  "weight"           { $$ = MAKE(weights, $1); }
| "weight" weights   { $$ = MAKE(weights, $1, $2); }
;

%%

void
rat_exp_parse_::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

int main()
{
  rat_exp_parse_::parser p;
  extern int yy_flex_debug;
  yy_flex_debug = !!getenv("YYSCAN");
  p.set_debug_level(!!getenv("YYDEBUG"));
  return p.parse();
}
