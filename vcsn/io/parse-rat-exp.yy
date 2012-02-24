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
  #include <core/rat_exp/node.hh>

  union YYSTYPE
  {
    int ival;
    std::string* sval;
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

%token  <sval>   WEIGHT  "weight"
                 WORD    "word"
;

%type <nodeval> exp

%nonassoc RWEIGHT
%nonassoc "(" ")" ONE ZERO
%left "+"
%nonassoc WORD
%left "."
%nonassoc "weight"
%nonassoc LWEIGHT
%nonassoc CONCAT
%nonassoc "*"


%start exp
%%

exp :
"(" exp ")"                             {
  $$ = $2;
 }
| exp "*"                               {
  $$ = new vcsn::rat_exp::kleene($1);
  }
| exp "." exp                           {
  vcsn::rat_exp::concat *tmp = new vcsn::rat_exp::concat();
  tmp->push_front($1);
  tmp->push_front($3);
  $$ = tmp;
  }
| exp "+" exp                           {
  vcsn::rat_exp::plus *tmp = new vcsn::rat_exp::plus();
  tmp->push_front($1);
  tmp->push_front($3);
  $$ = tmp;
  }
| exp exp       %prec CONCAT            {
  vcsn::rat_exp::concat *tmp = new vcsn::rat_exp::concat();
  tmp->push_front($1);
  tmp->push_front($2);
  $$ = tmp;
 }
| "weight" exp  %prec LWEIGHT           {
  $$ = new vcsn::rat_exp::left_weight($1, $2);
  }
| exp "weight"  %prec RWEIGHT           {
  $$ = new vcsn::rat_exp::right_weight($1, $2);
  }
| ONE                                   {
  $$ = new vcsn::rat_exp::one();
  }
| ZERO                                  {
  $$ = new vcsn::rat_exp::zero();
  }
| WORD                                  {
  $$ = new vcsn::rat_exp::word($1);
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
