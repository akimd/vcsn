// -*- mode: c++ -*-
%require "2.5"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define namespace "vcsn::rat"
%name-prefix "vcsn::rat::"

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <core/rat/node.hh>
  #include <core/rat/factory.hh>
  #include <core/rat/printer.hh>
}

%code provides
{
  namespace vcsn
  {
    namespace rat
    {
      int
      lex(parser::semantic_type* yylval, parser::location_type* yylloc);
    }
  }
  #define YY_DECL                                               \
    int                                                         \
    vcsn::rat::lex(vcsn::rat::parser::semantic_type* yylval,    \
                   vcsn::rat::parser::location_type* yylloc)
}

%code
{
  #include <cassert>
  #include <sstream>
  #include <weights/z.hh>

  typedef std::string weight_str;
  typedef std::list<weight_str*> weight_str_container;

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
    namespace rat
    {

      // Define the factory.
      factory* fact;
#define MAKE(Kind, ...)                         \
      fact->op_ ## Kind(__VA_ARGS__)

      exp* result;
    }
  }
}

%union
{
  int ival;
  std::string* sval;
  std::list<std::string*>* weights;
  exp* node;
};

%printer { debug_stream() << $$; } <ival>;
%printer { debug_stream() << *$$; } <sval> <weights>; // FIXME: <node>;
%destructor { delete $$; } <sval> <weights> <node>;

%token <ival>   LPAREN  "("
                RPAREN  ")"
%token  PLUS  "+"
        DOT   "."
        STAR  "*"
        ONE   "\\e"
        ZERO  "\\z"
;

%token  <sval> ATOM    "atom"
               WEIGHT  "weight";

%type <weights> weights weights.opt;
%type <node> exp term lexp factor leaf factors;


%left "+"
%left "."

%start exps
%%

exps:
  exp  { result = $1; }
;

exp:
  term                          { $$ = $1; }
| exp "." exp                   { $$ = MAKE(prod, $1, $3); }
| exp "+" exp                   { $$ = MAKE(sum, $1, $3); }
;

term:
  lexp weights.opt              { $$ = MAKE(weight, $1, $2); }
;

lexp:
  weights.opt factors           { $$ = MAKE(weight, $1, $2); }
| lexp weights factors          { $$ = MAKE(prod, $1, MAKE(weight, $2, $3)); }
;

factors:
  factor                        { $$ = $1; }
| factors factor                { $$ = MAKE(prod, $1, $2); }
;

factor:
  leaf                          { $$ = $1; }
| factor "*"                    { $$ = MAKE(star, $1); }
| "(" exp ")"                   { $$ = $2; assert($1 == $3); }
;

leaf:
  ZERO                          { $$ = MAKE(zero); }
| ONE                           { $$ = MAKE(one); }
| ATOM                          { $$ = MAKE(atom, $1); }
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

extern int yy_flex_debug;
namespace vcsn
{
  namespace rat
  {
    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      std::cerr << l << ": " << m << std::endl;
    }

    template <typename WeightSet>
    static
    exp*
    parse()
    {
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      parser p;
      factory_<weightset_t> zfact; // FIXME: specialization
      fact = &zfact;
      yy_flex_debug = !!getenv("YYSCAN");
      p.set_debug_level(!!getenv("YYDEBUG"));
      result = 0;
      if (p.parse())
        return 0;
      else
        return result;
    }
  }
}

int
main()
{
  typedef vcsn::z weightset_t;
  typedef typename weightset_t::value_t weight_t;
  if (vcsn::rat::exp* e = vcsn::rat::parse<weightset_t>())
    {
      const auto* down = down_cast<const vcsn::rat::node<weight_t>*>(e);
      vcsn::rat::printer<weightset_t>
        print(std::cout, weightset_t(), true, true);
      down->accept(print);
      std::cout << std::endl;
      return 0;
    }
  return 1;
}
