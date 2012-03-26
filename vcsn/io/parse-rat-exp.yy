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
  #include <vcsn/core/rat/node.hh>
  #include <vcsn/core/rat/factory_.hh>
  #include <vcsn/core/rat/printer.hh>
}

%code provides
{
  extern int yy_flex_debug;

  #define YY_DECL                                               \
    int                                                         \
    vcsn::rat::lex(vcsn::rat::parser::semantic_type* yylval,    \
                   vcsn::rat::parser::location_type* yylloc)

  namespace vcsn
  {
    namespace rat
    {
      int
      lex(parser::semantic_type* yylval, parser::location_type* yylloc);

      template <typename WeightSet>
      exp*
      parse()
      {
        typedef WeightSet weightset_t;
        typedef typename weightset_t::value_t weight_t;

        // Factory.
        extern factory* fact;
        factory_<weightset_t> f;
        fact = &f;

        // Scanner.
        yy_flex_debug = !!getenv("YYSCAN");

        // Parser.
        parser p;
        p.set_debug_level(!!getenv("YYDEBUG"));
        extern exp* result;
        result = 0;
        if (p.parse())
          return 0;
        else
          return result;
      }
    }
  }
}

%code
{
  #include <cassert>
  #include <sstream>

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
      factory* fact;
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
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
%printer
{
  if ($$)
    debug_stream() << '[' << *$$ << ']';
  else
    debug_stream() << "nullptr";
} <weights>; // FIXME: <node>;
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
| exp "." exp                   { $$ = fact->mul($1, $3); }
| exp "+" exp                   { $$ = fact->add($1, $3); }
;

term:
  lexp weights.opt              { $$ = fact->weight($1, $2); }
;

lexp:
  weights.opt factors           { $$ = fact->weight($1, $2); }
| lexp weights factors          { $$ = fact->mul($1, fact->weight($2, $3)); }
;

factors:
  factor                        { $$ = $1; }
| factors factor                { $$ = fact->mul($1, $2); }
;

factor:
  leaf                          { $$ = $1; }
| factor "*"                    { $$ = fact->star($1); }
| "(" exp ")"                   { $$ = $2; assert($1 == $3); }
;

leaf:
  ZERO                          { $$ = fact->zero(); }
| ONE                           { $$ = fact->unit(); }
| ATOM                          { $$ = fact->atom($1); }
;

weights.opt:
  /* empty */                   { $$ = nullptr; }
| weights                       { $$ = $1; }
;

weights:
  "weight"                      { $$ = fact->weight($1); }
| "weight" weights              { $$ = fact->weight($1, $2); }
;

%%

namespace vcsn
{
  namespace rat
  {
    void
    vcsn::rat::parser::error(const location_type& l, const std::string& m)
    {
      std::cerr << l << ": " << m << std::endl;
    }
  }
}
