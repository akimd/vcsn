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

%parse-param { driver& driver_ }
%lex-param   { driver& driver_ }

%initial-action
{
  @$ = driver_.location_;
}

%union
{
  std::string* sval = 0;
};

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
  SEMI     ";"
;
%token <sval> ID;
%type <sval> id.opt;
%printer { debug_stream() << '"' << *$$ << '"'; } <sval>;
%destructor { delete $$; } <sval>;

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
| ID "=" ID {}
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
  /* empty. */ {}
| attr_list
;

a_list.1:
  ID                   {}
| ID "=" ID            {}
| ID         a_list.1  {}
| ID "=" ID  a_list.1  {}
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
