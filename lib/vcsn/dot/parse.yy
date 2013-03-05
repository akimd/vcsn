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
%define location_type "vcsn::rat::location"
%name-prefix "vcsn::dot::"

%code requires
{
  #include <iostream>
  #include <string>

  #include <lib/vcsn/rat/location.hh>
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
        // Identifiers (attributes and node names).
        using string_t = std::string;
        string_t string;
        // A set of states.
        using states_t = std::vector<std::string>;
        states_t states;
        // (Unlabeled) transitions.
        using transitions_t = std::vector<std::pair<std::string, std::string>>;
        transitions_t transitions;
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

%code
{
#include <cassert>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/misc/echo.hh>

  namespace vcsn
  {
    namespace dot
    {
      std::ostream&
      operator<<(std::ostream& o, const sem_type::states_t ss)
      {
        bool first = true;
        o << '{';
        for (auto s: ss)
          {
            if (!first)
              o << ", ";
            o << s;
            first = false;
          }
        return o << '}';
      }

      std::ostream&
      operator<<(std::ostream& o, const sem_type::transitions_t ts)
      {
        bool first = true;
        o << '{';
        for (auto t: ts)
          {
            if (!first)
              o << ", ";
            o << t.first << "->" << t.second;
            first = false;
          }
        return o << '}';
      }
    }
  }

  /// Run Stm, and bounces exceptions into parse errors at Loc.
#define TRY(Loc, Stm)                           \
  try                                           \
    {                                           \
      Stm;                                      \
    }                                           \
  catch (std::exception& e)                     \
    {                                           \
      error(Loc, e.what());                     \
      YYERROR;                                  \
    }
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

%token <string> ID;
%type <string> id.opt;

// A single state.
%type <string> node_id node_stmt;
%printer { debug_stream() << $$; } <string>;

// Set of nodes, including for subgraphs.
%type <states> nodes stmt stmt_list edge_stmt subgraph;
%printer { debug_stream() << $$; } <states>;

// Rational expressions labeling the edges.
%type <string> attr_assign a a_list.0 a_list.1 attr_list attr_list.opt;
%%

graph:
  "digraph" id.opt "{" stmt_list "}"
  {
  }
;

stmt_list:
  /* empty. */  {}
| stmt_list stmt semi.opt
  {
    // Preserve the set of states.
    std::swap($$, $1);
    for (auto s: $2)
      $$.emplace_back(std::move(s));
  }
;

stmt:
  node_stmt
  {
    $$.emplace_back(std::move($1));
  }
| edge_stmt
  {
    std::swap($$, $1);
  }
| attr_stmt
  {
  }
| attr_assign
  {
  }
| subgraph
  {
    std::swap($$, $1);
  }
;

attr_stmt:
  "graph" attr_list  {}
| "node"  attr_list  {}
| "edge"  attr_list  {}
;

attr_list:
  "[" a_list.0 "]" attr_list.opt
  {
    std::swap($$, $2.empty() ? $4 : $2);
  }
;

attr_list.opt:
  // This action seems useless, but because Bison initializes $$ with $0
  // there are very surprising results...
  /* empty. */ { $$.clear(); }
| attr_list    { std::swap($$, $1); }
;

attr_assign:
  ID[var] "=" ID[val]
  {
    if ($var == "label")
      std::swap($$, $val);
    else if ($var == "vcsn_context")
      {
        assert(!driver_.edit_);
        std::swap(driver_.context_, $val);
      }
  }

// A single attribute.  Keep only labels.
a:
  ID           { $$.clear(); }
| attr_assign  { std::swap($$, $1); }
;

// One or more attributes.
a_list.1:
  a comma.opt a_list.0
  {
    std::swap($$, $1.empty() ? $3 : $1);
  }
;

comma.opt:
  /* empty. */
| ","
;

semi.opt:
  /* empty. */
| ";"
;

// Zero or more attributes.
a_list.0:
  /* empty. */ {}
| a_list.1     { std::swap($$, $1); }
;

nodes:
  node_id   { $$.emplace_back(std::move($node_id)); }
| subgraph  { $$ = $<states>subgraph; }
;

// Transform the right-recursion from the original grammar into a left
// one, because parsing 1->2->3 as 1(->2)(->3) is ugly and painful to
// handle (we must maintain some state to know what is the current
// source, 1, when arriving to ->2, and 2 when arriving to ->3).
//
// ((1->2)->3 is more natural.
//
// We "cheat": when maintaining a path, we flatten it in two
// components: the total set of transitions (as a list of transitions of
// states), and the set of ending states (using the <states> field
// selector), as they will be used as set of starting states if there
// is another "->" afterward.
%type <transitions> path;
%printer { debug_stream() << $$ /* << " (states: " << $<states>$ << ')' */; }
  <transitions>;
path:
  nodes[from] "->" nodes[to]
  {
    for (auto s1: $from)
      for (auto s2: $to)
        $$.emplace_back(s1, s2);
    $<states>$ = $to;
  }
| path[from]  "->" nodes[to]
  {
    std::swap($$, $from);
    for (auto s1: $<states>from)
      for (auto s2: $to)
        $<transitions>$.emplace_back(s1, s2);
    $<states>$ = $to;
  }
;

edge_stmt:
  path attr_list.opt[label]
  {
    for (auto t: $path)
      driver_.edit_->add_entry(t.first, t.second, $label);
  }
;

node_stmt:
  node_id attr_list.opt
  {
    std::swap($$, $1);
  }
;

node_id:
  ID
  {
    // We need the editor to exist.
    TRY(@$, driver_.setup_());
    std::swap($$, $1);
    if ($$[0] == 'I')
      driver_.edit_->add_pre($$);
    else if ($$[0] == 'F')
      driver_.edit_->add_post($$);
    else
      // This is not needed, but it ensures that the states will be
      // numbered by their order of appearance in the file.
      driver_.edit_->add_state($$);
  }
;

subgraph:
  "subgraph" id.opt "{" stmt_list "}"  { std::swap($$, $stmt_list); }
|                   "{" stmt_list "}"  { std::swap($$, $stmt_list); }
;

id.opt:
  /* empty. */ {}
| ID           { std::swap($$, $1); }
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
