// -*- mode: c++ -*-
%require "2.5"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%name-prefix "rat_exp_parse_"

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <core/rat_exp/node.hh>
  #include <core/rat_exp/RatExpFactory.hh>
  #include <core/rat_exp/print_debug_visitor.hh>

  union YYSTYPE
  {
    int ival;
    std::string* sval;
    vcsn::rat_exp::weight_type* weight;
    vcsn::rat_exp::weights_type* weights;
    vcsn::rat_exp::RatExp* nodeval;
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

  typedef vcsn::rat_exp::weight_type weight_type;
  typedef vcsn::rat_exp::weights_type weights_type;

  namespace std
  {
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
  }

  struct foo
  {
    typedef int value_t;
    static int one() { return 1; }
    static int zero() { return 0; }
    static void op_mul_eq(int i, std::string *str) {
      i *= atoi(str->c_str());
    }
  }; // FIXME

  namespace vcsn
  {
    namespace rat_exp
    {
      std::ostream&
      operator<<(std::ostream& o, const RatExp& r)
      {
        auto down = down_cast<const vcsn::rat_exp::RatExpNode<foo>*>(&r);
        vcsn::rat_exp::PrintDebugVisitor<foo> print(o);
        down->accept(print);
        return o;
      }
    }
  }

  // define the factory
  vcsn::rat_exp::RatExpFactory<foo> fact; // FIXME: specialization
  #define MAKE(Kind, ...)                         \
    fact.op_ ## Kind(__VA_ARGS__)
}

%printer { debug_stream() << $$; } <ival>;
%printer { debug_stream() << *$$; } <sval> <weight> <weights> <nodeval>;
%destructor { delete $$; } <sval> <weight> <weights> <nodeval>;

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
%type <nodeval> exps exp term lexp factor word factors;


%left "+"
%left "."

%start exps
%%

exps:
  exp
  {
    $$ = fact.cleanNode($1);
    std::cout << *$$;
  }
;

exp:
  term                          { $$ = $1; }
| exp "." exp                   { $$ = MAKE(mul, $1, $3); }
| exp "+" exp                   { $$ = MAKE(add, $1, $3); }
;

term:
  lexp weights.opt              { $$ = $2 ? MAKE(weight, $1, $2) : $1; }
;

lexp:
  weights.opt factors           { $$ = MAKE(weight, $1, $2); }
| lexp weights factors          { $$ = MAKE(mul, $1, MAKE(weight, $2, $3)); }
;

factors:
  factor                        { $$ = MAKE(mul, $1); }
| factors factor                { $$ = MAKE(mul, $1, $2); }
;

factor:
  word                          { $$ = $1; }
| factor "*"                    { $$ = MAKE(kleene, $1); }
| "(" exp ")"                   { $$ = fact.cleanNode($2); assert($1 == $3); }
;

word:
  ZERO                          { $$ = MAKE(zero); }
| ONE                           { $$ = MAKE(one); }
| WORD                          { $$ = MAKE(word, $1); }
;

weights.opt:
  /* empty */                   { $$ = nullptr; }
| weights                       { $$ = $1; }
;

weights:
  "weight"                      { $$ = MAKE(weight, $1); }
| "weight" weights              { $$ = MAKE(weight, $1, $2); }
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
