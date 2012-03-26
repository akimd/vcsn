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
  #include <vcsn/core/rat/factory.hh>
}

%code provides
{
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

      /// Prepare scanner to load file f.
      void scan_file(const std::string& f);
      /// Prepare scanner to read string e.
      void scan_string(const std::string& e);
      /// Close input.
      void scan_close();

      exp* parse_file(const std::string& f, const factory& fact);

      exp* parse_string(const std::string& e, const factory& fact);
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
      const factory* fact;
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

    static
    exp*
    parse(const factory& f)
    {
      // Factory.
      extern const factory* fact;
      const factory* old_fact = fact;
      fact = &f;

      // Parser.
      parser p;
      p.set_debug_level(!!getenv("YYDEBUG"));
      extern exp* result;
      exp* res = 0;
      std::swap(result, res);
      if (p.parse())
        result = 0;
      scan_close();
      fact = old_fact;
      std::swap(result, res);
      return res;
    }

    exp*
    parse_file(const std::string& f, const factory& fact)
    {
      scan_file(f);
      return parse(fact);
    }

    exp*
    parse_string(const std::string& e, const factory& fact)
    {
      scan_string(e);
      return parse(fact);
    }
  }
}
