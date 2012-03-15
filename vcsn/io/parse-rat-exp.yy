// -*- mode: c++ -*-
%require "2.5"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define namespace "vcsn::rat_exp"
%name-prefix "vcsn::rat_exp::"

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <core/rat_exp/node.hh>
  #include <core/rat_exp/RatExpFactory.hh>
  #include <core/rat_exp/print_debug_visitor.hh>
}

%code provides
{
  namespace vcsn
  {
    namespace rat_exp
    {
      int
      lex(parser::semantic_type* yylval, parser::location_type* yylloc);
    }
  }
  #define YY_DECL                                                       \
    int                                                                 \
    vcsn::rat_exp::lex(vcsn::rat_exp::parser::semantic_type* yylval,    \
                     vcsn::rat_exp::parser::location_type* yylloc)
}

%code
{
  #include <cassert>
  #include <sstream>
  #include <weights/z.hh>

  typedef std::string weight_str;
  typedef std::list<weight_str *> weight_str_container;

  namespace std
  {
    std::ostream&
    operator<<(std::ostream& o, const weight_str_container& ws)
    {
      o << "{";
      bool first = true;
      for (auto w: ws)
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

  namespace vcsn
  {
    namespace rat_exp
    {
      struct foo
      {
        typedef int value_t;
        static int one() { return 1; }
        static bool is_unit(const int i) { return i == 1; }
        static bool is_zero(const int i) { return i == 0; }
        static int zero() { return 0; }
        static void op_mul_eq(int &i, std::string *str) {
          i *= atoi(str->c_str());
        }
        static int op_conv(std::string *str) {
          return atoi(str->c_str());
        }
      }; // FIXME

      std::ostream&
      operator<<(std::ostream& o, const RatExp& r)
      {
        const RatExpNode<foo>* down = down_cast<const RatExpNode<foo>*>(&r);
        PrintDebugVisitor<foo> print(o);
        down->accept(print);
        return o;
      }

      // Define the factory.
      RatExpFactory<vcsn::z> fact; // FIXME: specialization
#define MAKE(Kind, ...)                         \
      fact.op_ ## Kind(__VA_ARGS__)
    }
  }

  // define the factory
  vcsn::rat_exp::RatExpFactory<vcsn::z> fact; // FIXME: specialization
  #define MAKE(Kind, ...)                         \
    fact.op_ ## Kind(__VA_ARGS__)
}

%union
{
  int ival;
  std::string* sval;
  std::string* weight;
  std::list<std::string*>* weights;
  RatExp* nodeval;
};

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
    $$ = fact.clean_node($1);
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
| "(" exp ")"                   { $$ = fact.clean_node($2); assert($1 == $3); }
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
vcsn::rat_exp::parser::error(const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

int main()
{
  vcsn::rat_exp::parser p;
  extern int yy_flex_debug;
  yy_flex_debug = !!getenv("YYSCAN");
  p.set_debug_level(!!getenv("YYDEBUG"));
  return p.parse();
}
