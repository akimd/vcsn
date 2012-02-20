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

  #define YY_DECL\
    int rat_exp_parse_lex(YYSTYPE* yylval, const rat_exp_parse_::location*)
  YY_DECL;
}

%code
{
  #include <cassert>
  #include <sstream>
  #define MAKE(Out, In)                         \
    do {                                        \
      std::stringstream o;                      \
      o << '(' << In << ')';                    \
      Out = new std::string(o.str());           \
    } while (false)
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

%type <sval> exp

%nonassoc RWEIGHT
%nonassoc "(" ")" ONE ZERO
%left "+"
%nonassoc WORD
%left "."
%nonassoc "weight"
%nonassoc LWEIGHT
%nonassoc CONCAT
%nonassoc "*"


%start exps
%%

exps : exp { std::cout << *$1 << std::endl; }
;

exp :
"(" exp ")"                    { $$ = $2; assert($1 == $3); }
| exp "*"                      { MAKE($$, *$1 << '*'); }
| exp "." exp                  { MAKE($$, *$1 << '.' << *$3); }
| exp "+" exp                  { MAKE($$, *$1 << "<<" << *$3); }
| exp exp       %prec CONCAT   { MAKE($$, *$1 << '#' << *$2); }
| "weight" exp  %prec LWEIGHT  { MAKE($$, "{l" << *$1 << '}' << *$2 ); }
| exp "weight"  %prec RWEIGHT  { MAKE($$, *$1 << "{r" << *$2 << '}'); }
| ONE                          { MAKE($$, ("\\e")); }
| ZERO                         { MAKE($$, ("\\z")); }
| WORD                         { MAKE($$, *$1); }
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
