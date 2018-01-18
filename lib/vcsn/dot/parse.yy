// -*- mode: c++ -*-
// See <http://www.graphviz.org/content/dot-language>.
%require "3.0"
%language "C++"

%debug
%defines
%error-verbose
%expect 0
%locations
%define api.namespace {vcsn::detail::dot}
%define api.value.type variant
%define api.location.type {vcsn::rat::location}
%define api.token.constructor

%code requires
{
  #include <iostream>
  #include <string>

  #include <vcsn/misc/location.hh>
  #include <vcsn/misc/symbol.hh>
  #include <lib/vcsn/dot/driver.hh>

  namespace vcsn
  {
    namespace detail
    {
      namespace dot
      {
        // Identifiers (attributes and node names).
        //
        // With ref_counting I have a 20% penalty compared to using
        // std::string.  With no_tracking, I get a 10x speed up (e.g.,
        // vcsn-cat of a determinized ladybird).  But then we leak.
        // We should try to have an arena in which the flyweight
        // performs its allocation, and flush the whole arena once
        // we're done parsing.
        using string_t = symbol;

        // A set of states.
        using states_t = std::vector<string_t>;

        // (Unlabeled) transitions.
        using transitions_t = std::vector<std::pair<string_t, string_t>>;

        // A set of paths.
        struct paths_t
        {
          transitions_t transitions;
          // The current ends of paths.  Not the same as the
          // destinations of all the transitions, e.g., in the case of
          // "0 -> 1 -> 2", transitions = { 0 -> 1, 1 -> 2 }, ends = {
          // 2 }.
          states_t ends;
        };
      }
    }
  }
}

%code provides
{
  #define YY_DECL_(Class) \
    parser::symbol_type Class lex()
  #define YY_DECL YY_DECL_(yyFlexLexer::)
}


%code
{
  #include <cassert>
  #include <vcsn/algos/edit-automaton.hh>
  #include <lib/vcsn/dot/scan.hh>

  namespace vcsn
  {
    namespace detail
    {
      namespace dot
      {
        static std::ostream&
        operator<<(std::ostream& o, const states_t ss)
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

        static std::ostream&
        operator<<(std::ostream& o, const paths_t ps)
        {
          bool first = true;
          o << '{';
          for (auto t: ps.transitions)
            {
              if (!first)
                o << ", ";
              o << t.first << "->" << t.second;
              first = false;
            }
          return o << '}';
        }

        /// Use our local scanner object.
        static
        inline
        parser::symbol_type
        yylex(driver& driver_)
        {
          return driver_.scanner_->lex();
        }
      }
    }
  }
}

%param { driver& driver_ }

%code top
{
  /// Run Stm, and bounces exceptions into parse errors at Loc.
#define TRY(Loc, Stm)                           \
  do {                                          \
    try                                         \
      {                                         \
        Stm;                                    \
      }                                         \
    catch (std::exception& e)                   \
      {                                         \
        error(Loc, e.what());                   \
        YYERROR;                                \
      }                                         \
  } while (false)
}

%initial-action
{
  @$ = driver_.location_;
}

%token
  DIGRAPH  "digraph"
  EDGE     "edge"
  GRAPH    "graph"
  NODE     "node"
  SUBGRAPH "subgraph"
  LBRACE   "{"
  RBRACE   "}"
  LBRACKET "["
  RBRACKET "]"
  EQ       "="
  ARROW    "->"
  COLON    ":"
  COMMA    ","
  SEMI     ";"
  END      0 "end"
;

%token <string_t> ID "identifier";
%type <string_t> id.opt;

// A single state.
%type <string_t> node_id node_stmt;
%printer { debug_stream() << $$; } <string_t>;

// Set of nodes, including for subgraphs.
%type <states_t> nodes stmt stmt_list edge_stmt subgraph;
%printer { debug_stream() << $$; } <states_t>;

// Rational expressions labeling the edges.
%type <string_t> attr_assign a_list.0 a_list.1 attr_list attr_list.opt;
%%

graph:
  "digraph" id.opt "{" stmt_list "}"    {}
;

stmt_list:
  %empty        {}
| stmt_list stmt semi.opt
  {
    // Preserve the set of states.
    std::swap($$, $1);
    for (auto s: $2)
      $$.emplace_back(std::move(s));
  }
;

stmt:
  node_stmt   { $$.emplace_back(std::move($1)); }
| edge_stmt   { std::swap($$, $1); }
| attr_stmt   {}
| attr_assign {}
| subgraph    { std::swap($$, $1); }
| error       {}
;

attr_stmt:
  "graph" attr_list  {}
| "node"  attr_list  {}
| "edge"  attr_list  {}
;

attr_list:
  "[" a_list.0 "]" attr_list.opt
  {
    std::swap($$, $2.get().empty() ? $4 : $2);
  }
;

attr_list.opt:
  // This action seems useless, but because Bison initializes $$ with $0
  // there are very surprising results...
  %empty       { $$ = ""; }
| attr_list    { std::swap($$, $1); }
;

attr_assign:
  ID[var] "=" ID[val]
  {
    static const auto label = string_t{"label"};
    static const auto ctx = string_t{"vcsn_context"};
    if ($var == label)
      std::swap($$, $val);
    else if ($var == ctx)
      driver_.setup_(@val, $val);
    else
      // Beware of the default "$$ = $1;" action.
      $$ = "";
  }

// One or more attributes.
a_list.1:
  attr_assign comma.opt a_list.0
  {
    std::swap($$, $1.get().empty() ? $3 : $1);
  }
;

comma.opt:
  %empty | "," | ";"
;

semi.opt:
  %empty
| ";"
;

// Zero or more attributes.
a_list.0:
  %empty       {}
| error        {}
| a_list.1     { std::swap($$, $1); }
;

nodes:
  node_id   { $$.emplace_back(std::move($node_id)); }
| subgraph  { $$ = $subgraph; }
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
%type <paths_t> path;
%printer { debug_stream() << $$; } <paths_t>;
path:
  nodes[from] "->" nodes[to]
  {
    for (auto s1: $from)
      for (auto s2: $to)
        $$.transitions.emplace_back(s1, s2);
    $$.ends = $to;
  }
| path[from]  "->" nodes[to]
  {
    std::swap($$.transitions, $from.transitions);
    for (auto s1: $from.ends)
      for (auto s2: $to)
        $$.transitions.emplace_back(s1, s2);
    $$.ends = $to;
  }
;

edge_stmt:
  path attr_list.opt[label]
  {
    if (driver_.has_edit_(@1))
      for (auto t: $path.transitions)
        TRY(@2, driver_.edit_->add_entry(t.first, t.second, $label));
  }
;

node_stmt:
  node_id attr_list.opt
  {
    std::swap($$, $1);
  }
;

node_id:
  ID port.opt
  {
    std::swap($$, $1);
    if (driver_.has_edit_(@1))
      {
        if ($$.get()[0] == 'I')
          TRY(@1, driver_.edit_->add_pre($$));
        else if ($$.get()[0] == 'F')
          TRY(@1, driver_.edit_->add_post($$));
        else
          // This is not needed, but it ensures that the states will be
          // numbered by their order of appearance in the file.
          TRY(@1, driver_.edit_->add_state($$));
      }
  }
;

// Let's not bother with compass_pt.  Which means that we accept too
// many inputs: we should check that the last ID is
// (n|ne|e|se|s|sw|w|nw|c|_), but even dot itself accepts 'foo:bar'.
// Note that compass points are explicitly not reserved keywords.
port.opt:
  %empty        {}
| ":" ID        {}
| ":" ID ":" ID {}
;

subgraph:
  "subgraph" id.opt "{" stmt_list "}"  { std::swap($$, $stmt_list); }
|                   "{" stmt_list "}"  { std::swap($$, $stmt_list); }
;

id.opt:
  %empty       {}
| ID           { std::swap($$, $1); }
;
%%

namespace vcsn
{
  namespace detail
  {
    namespace dot
    {
      void
      parser::error(const location_type& l, const std::string& m)
      {
        driver_.error(l, m);
      }
    }
  }
}
