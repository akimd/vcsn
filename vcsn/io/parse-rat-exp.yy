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
  #include <string>
  #include "location.hh"

  union YYSTYPE
  {
    int ival;
    std::string* sval;
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

%token  <sval>   WEIGHT  "weight"
                 WORD    "word"
;

%type <sval> exp factor factors lexp term weights weights.opt word;

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
  lexp weights.opt { if (!$2->empty()) STRING($$, *$1 << "r{" << *$2 << '}'); }
;

lexp:
   weights.opt factors { if ($1->empty()) $$ = $2; else STRING($$, "l{" << *$1 << '}' << *$2); }
| lexp weights factors { STRING($$, *$1 << "#(l{" << *$2 << '}' << *$3 << ")"); }
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
  /* empty */ { $$ = new std::string(); }
| weights
;

weights:
  "weight"           { STRING_($$, *$1); }
| "weight" weights   { STRING_($$, *$1 << ", " << *$2); }
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
