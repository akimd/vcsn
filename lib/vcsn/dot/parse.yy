// -*- mode: c++ -*-
// See <http://www.graphviz.org/content/dot-language>.

%require "2.5"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define namespace "vcsn::dot"
%name-prefix "vcsn::dot::"

%code requires
{
  #include <iostream>
  #include <list>
  #include <string>
  #include "location.hh"
  #include <lib/vcsn/dot/driver.hh>

  namespace vcsn
  {
    namespace dot
    {

      // (Complex) objects such as shared_ptr cannot be put in a
      // union, even in C++11.  So cheat, and store a struct instead
      // of an union.  See lib/vcsn/rat/README.txt.
      struct sem_type
      {
        driver::exp_t exp;
        // These guys _can_ be put into a union.
        union
        {
          std::string* sval;
        };
      };
    }
  }
#define YYSTYPE vcsn::dot::sem_type
}

%code provides
{
  #define YY_DECL                                               \
    int                                                         \
    vcsn::dot::lex(vcsn::dot::parser::semantic_type* yylval,    \
                   vcsn::dot::parser::location_type* yylloc,    \
                   vcsn::dot::driver& driver_)

  namespace vcsn
  {
    namespace dot
    {
      int
      lex(parser::semantic_type* yylval,
          parser::location_type* yylloc,
          vcsn::dot::driver& driver_);
    }
  }
}

%code top
{
#include <vcsn/misc/echo.hh>
}

%parse-param { driver& driver_ }
%lex-param   { driver& driver_ }

%initial-action
{
  @$ = driver_.location_;
}

%token
  DIGRAPH  "digraph"
  EDGE     "edge"
  GRAPH    "graph"
  NODE     "node"
  LBRACE   "{"
  RBRACE   "}"
  LBRACKET "["
  RBRACKET "]"
  EQ       "="
  ARROW    "->"
  COMMA    ","
  SEMI     ";"
;

%token <sval> ID;
%type <sval> id.opt;
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
%destructor { delete $$; } <sval>;

%type <exp> attr_assign a a_list.0 a_list.1 attr_list attr_list.opt;
%printer
{
  if ($$)
    {
      assert(driver_.kratexpset_);
      driver_.kratexpset_->print(debug_stream(), $$);
    }
  else
    debug_stream() << "nullptr";
} <exp>;
%%

graph:
  "digraph" id.opt "{" stmt_list "}"  {}
;

stmt_list:
  /* empty. */
| stmt ";" stmt_list
| stmt     stmt_list
;

stmt:
  node_stmt
| edge_stmt
| attr_stmt
| attr_assign
| subgraph
;

attr_stmt:
  "graph" attr_list
| "node"  attr_list
| "edge"  attr_list
;

attr_list:
  "[" a_list.0 "]" attr_list.opt
;

attr_list.opt:
  /* empty. */ { $$ = 0; }
| attr_list    { $$ = $1; }
;

attr_assign:
  ID[var] "=" ID[val]
  {
    if (*$var == "label" && !$val->empty())
      {
        if (!driver_.kratexpset_)
          {
            if (driver_.context_.empty())
              {
                error(@$, "no vcsn_context defined");
                YYERROR;
              }
            if (driver_.letters_.empty())
              {
                error(@$, "no letters defined");
                YYERROR;
              }
            if (driver_.context_ == "char_b_lal")
              {
                auto ctx = new ctx::char_b_lal{driver_.letters_};
                driver_.kratexpset_ =
                  new concrete_abstract_kratexpset<vcsn::ctx::char_b_lal>{*ctx};
              }
            else
              {
                error(@$, "unknown context: " + driver_.context_);
                YYERROR;
              }
          }
        $$ = driver_.kratexpset_->conv(*$val);
      }
    else if (*$var == "vcsn_context")
      {
        assert(!driver_.kratexpset_);
        std::swap(driver_.context_ , *$val);
      }
    else if (*$var == "vcsn_genset")
      {
        assert(!driver_.kratexpset_);
        for (auto l: *$val)
          driver_.letters_.insert(l);
      }
    delete $1;
    delete $3;
  }

a:
  ID           { $$ = 0; delete $1; }
| attr_assign  { $$ = $1; }
;

a_list.1:
  a comma.opt a_list.0  { if ($1) std::swap($$, $1); else std::swap($$, $3); }
;

comma.opt:
  /* empty. */
| ","
;

a_list.0:
  /* empty. */ {}
| a_list.1
;

edge_stmt:
  node_id  edgeRHS attr_list.opt
| subgraph edgeRHS attr_list.opt

edgeRHS:
  "->" node_id  edgeRHS.opt
| "->" subgraph edgeRHS.opt
;

edgeRHS.opt:
  /* empty. */ {}
| edgeRHS
;

node_stmt:
  node_id attr_list.opt
;

node_id:
  ID
;

subgraph:
  "subgraph" id.opt "{" stmt_list "}"  {}
|                   "{" stmt_list "}"  {}
;

id.opt:
  /* empty. */ { $$ = new std::string; }
| ID           { $$ = $1; }
;
%%

namespace vcsn
{
  namespace dot
  {
    void
    vcsn::dot::parser::error(const location_type& l, const std::string& m)
    {
      driver_.error(l, m);
    }
  }
}
