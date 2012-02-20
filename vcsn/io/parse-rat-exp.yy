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
  #include <core/rat_exp/node.hh>

  union YYSTYPE
  {
    int ival;
    std::string* sval;
    vcsn::rat_exp::weight_type* weight;
    vcsn::rat_exp::weights_type* weights;
    vcsn::rat_exp::exp *nodeval;
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

}

%printer { debug_stream() << *$$; } <sval>;
%printer { debug_stream() << $$; } <ival>;

%destructor { delete $$; } <sval>;

%destructor { delete $$; } <rat_exp *> <rat_concat *> <rat_plus *> <rat_kleene *>
        <rat_one *> <rat_zero *> <rat_atom *> <rat_word *> <rat_left_weight *>
        <rat_right_weight *>

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

// FIXME: check
%type <weights> weights weights.opt;
%type <nodeval> exp term lexp factors factor word;


%left "+"
%left "."

%start exp
%%

exp:
  term                          {
  $$ = $1;
 }
| exp "." exp                   {
  vcsn::rat_exp::concat *tmp = new vcsn::rat_exp::concat();
  tmp->push_front($1);
  tmp->push_front($3);
  $$ = tmp;
 }
| exp "+" exp                   {
  vcsn::rat_exp::plus *tmp = new vcsn::rat_exp::plus();
  tmp->push_front($1);
  tmp->push_front($3);
  $$ = tmp;
  }
;

term:
  lexp weights.opt              {
    if($2 != nullptr)
      $$ = new vcsn::rat_exp::right_weight($1, $2);
    else
      $$ = $1;
  }
;

lexp:
  weights.opt factors           {
    if($1 != nullptr)
      $$ = new vcsn::rat_exp::left_weight($1, $2);
    else
      $$ = $2;
  }
| lexp weights factors          {
  vcsn::rat_exp::left_weight *right = new vcsn::rat_exp::left_weight($2, $3);
  vcsn::rat_exp::concat *tmp = new vcsn::rat_exp::concat();
  tmp->push_front(right);
  tmp->push_front($1);
  $$ = tmp;
  }
;

factors:
  factor                        {
    vcsn::rat_exp::concat *tmp = new vcsn::rat_exp::concat();
    tmp->push_front($1);
    $$ = $1;
  }
| factors factor                {
  vcsn::rat_exp::concat *tmp = dynamic_cast<vcsn::rat_exp::concat *> ($2);
  assert(tmp != nullptr);
  tmp->push_front($1);
  $$ = tmp;
}
;

factor:
  word                          {
    $$ = $1;
  }
| factor "*"                    {
  $$ = new vcsn::rat_exp::kleene($1);
 }
| "(" exp ")"                   {
 $$ = $2; assert($1 == $3);
  }
;

word:
  ZERO                          {
    $$ = new vcsn::rat_exp::zero();
  }
| ONE                           {
  $$ = new vcsn::rat_exp::one();
  }
| WORD                          {
  $$ = new vcsn::rat_exp::word($1);
  }
;

weights.opt:
  /* empty */                   {
  $$ = nullptr;
}
| weights                       {
  $$ = $1;
  }
;

weights:
  "weight"                      {
    $$ = new weights_type();
    $$->push_front($1);
  }
| "weight" weights              {
  $$ = $2;
  $$->push_front($1);
  }
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
