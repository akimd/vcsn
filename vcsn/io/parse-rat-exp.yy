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
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <core/rat_exp/node.hh>
  #include <core/rat_exp/rat-exp.hh>
  #include <core/rat_exp/print_debug_visitor.hh>

  union YYSTYPE
  {
    int ival;
    std::string* sval;
    vcsn::rat_exp::weight_type* weight;
    vcsn::rat_exp::weights_type* weights;
    vcsn::rat_exp::exp *nodeval;
    vcsn::rat_exp::concat *concatval;
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

  typedef vcsn::rat_exp::weight_type weight_type;
  typedef vcsn::rat_exp::weights_type weights_type;

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

  // define the factory
  vcsn::rat_exp::RatExp<int> fact; // FIXME: specialization
}

%printer { debug_stream() << *$$; } <sval>;
%printer { debug_stream() << $$; } <ival>;

%destructor { delete $$; } <sval>;

%destructor { delete $$; } <rat_exp *> <rat_concat *> <rat_plus *>
        <rat_kleene *> <rat_one *> <rat_zero *> <rat_atom *> <rat_word *>
        <rat_left_weight *> <rat_right_weight *>

%token <ival>   LPAREN          "("
                RPAREN          ")"
%token  PLUS            "+"
        DOT             "."
        STAR            "*"
        ONE             "\\e"
        ZERO            "\\z"
;

%token  <sval> WORD    "word";
%token <weight> WEIGHT  "weight"

%type <weights> weights weights.opt;
%type <nodeval> exps exp term lexp factor word;
%type <concatval> factors;


%left "+"
%left "."

%start exps
%%

exps:
  exp                           {
    vcsn::rat_exp::PrintDebugVisitor print(std::cout);
    $1->accept(print);
    $$ = $1;
 }

exp:
  term                          { $$ = $1; }
| exp "." exp                   { $$ = fact.op_mul($1, $3); }
| exp "+" exp                   { $$ = fact.op_add($1, $3); }
;

term:
  lexp weights.opt              {
    if($2 != nullptr)
      $$ = fact.op_weight($1, $2);
    else
      $$ = $1;
  }
;

lexp:
  weights.opt factors           {
    vcsn::rat_exp::exp *factors = $2;
    if(1 == $2->size())
    {
      factors = *$2->begin();
    }
    if($1 != nullptr)
      $$ = fact.op_weight($1, factors);
    else
      $$ = factors;
  }
| lexp weights factors          {
    vcsn::rat_exp::exp *factors = $3;
    if(1 == $3->size())
    {
      factors = *$3->begin();
    }
    vcsn::rat_exp::left_weight *right = fact.op_weight($2, factors);
    $$ = fact.op_mul($1, right);
  }
;

factors:
  factor                        { $$ = fact.op_mul($1); }
| factors factor                { $$ = fact.op_mul($1, $2); }
;

factor:
  word                          { $$ = $1; }
| factor "*"                    { $$ = fact.op_kleene($1); }
| "(" exp ")"                   { $$ = $2; assert($1 == $3); }
;

word:
  ZERO                          { $$ = fact.op_zero(); }
| ONE                           { $$ = fact.op_one(); }
| WORD                          { $$ = fact.op_word($1); }
;

weights.opt:
  /* empty */                   { $$ = nullptr; }
| weights                       { $$ = $1; }
;

weights:
  "weight"                      { $$ = fact.op_weight($1); }
| "weight" weights              { $$ = fact.op_weight($1, $2); }
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
