#include <iostream>
#include <string>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <vcsn/misc/raise.hh>
#include <vcsn/misc/to-string.hh>

#include <lib/vcsn/dyn/config.hh>
#include <lib/vcsn/dyn/context-parser.hh>
#include <lib/vcsn/dyn/ctx-ast.hh>
#include <lib/vcsn/dyn/error-handler.hh>
#include <lib/vcsn/dyn/signature-printer.hh>

#define BOOST_SPIRIT_X3_DEBUG 1
#define BOOST_SPIRIT_DEBUG 1

// Public interface.
namespace vcsn::dyn::parser
{
  namespace x3 = boost::spirit::x3;
  namespace ascii = x3::ascii;

  // Name collisions with X3.
  struct ConText_class;
  using ConText_type = x3::rule<ConText_class, ast::ConText>;
  BOOST_SPIRIT_DECLARE(ConText_type);
}

namespace vcsn::dyn::parser
{
  using x3::eps;
  using x3::lit;
  using x3::attr;
  using x3::lexeme;
  using x3::string;
  using ascii::char_;
  using ascii::alnum;
  using ascii::alpha;
  using ascii::string;

  // Rule IDS.
  struct one_class;
  struct letterset_class;
  struct wordset_class;
  struct labelset_class;
  struct expressionset_class;
  struct seriesset_class;
  struct ConText_class;
  struct weightset_class;

  // Rules.
  x3::rule<one_class, ast::oneset> const
      oneset = "oneset";
  x3::rule<letterset_class, ast::letterset> const
      letterset = "letterset";
  x3::rule<wordset_class, ast::wordset> const
      wordset = "wordset";
  x3::rule<labelset_class, ast::labelset> const
      labelset = "labelset";
  x3::rule<expressionset_class, ast::expressionset> const
      expressionset = "expressionset";
  x3::rule<seriesset_class, ast::expressionset> const
      seriesset = "seriesset";
  x3::rule<ConText_class, ast::ConText> const
      ConText = "context";
  x3::rule<weightset_class, ast::weightset> const
      weightset = "weightset";

  auto mkseriesset
  = [](auto& ctx) { _val(ctx) = ast::expressionset{ast::ConText{_attr(ctx)}, "series"}; };

  // Grammar.
  const auto ConText_def = eps
    > labelset
    > (lit("->") | lit(","))
    > weightset
  ;

  const auto gens =
    lit('[') >> lexeme[*(char_ - ']')] >> lit(']')
    ;

  const auto gens_parens =
    lit('(') >> lexeme[*(char_ - ')')] >> lit(')')
    ;

  // <char>
  // <char(x)>
  // (x)
  const auto typed_gens =
    lit("<") >> (string("char") | string("string")) >> -gens_parens >> lit('>')
    | attr(std::string{"char"}) >> -gens_parens
    ;

  // <char>
  // eps
  const auto letter_type =
    lit("<") >> (string("char") | string("string")) >> lit('>')
    | attr(std::string{"char"})
    ;

  const auto oneset_def =
    lit("lao") >> !alnum
    ;

  const auto letterset_def
    = lit("lal") >> typed_gens >> !alnum
    | letter_type >> gens >> !char_('*')
   ;

  const auto wordset_def
    = lit("law") >> letter_type >> -gens_parens >> !alnum
    | letter_type >> gens >> lit('*')
    ;

  const auto labelset_def =
    oneset
    | letterset
    | wordset
    | lit("lat<") > (labelset % ',') > lit('>')
    | expressionset
    ;

  // An identifier must be separated from other alnum
  auto ident(const std::string& id)
  {
    return string(id) >> ! alnum;
  }

  const auto weightset_def =
    ident("b")
    | ident("f2")
    | ident("log")
    | ident("nmin")
    | ident("q")
    | ident("qmp")
    | ident("r")
    | ident("rmin")
    | ident("z")
    | ident("zmin")
    | expressionset
    | lit("lat<") > (weightset % ',') > lit('>')
    ;

  const auto seriesset_def =
    (lit("seriesset") > lit('<') > ConText > lit(">")) [mkseriesset]
    ;

  const auto expressionset_def =
    lit("expressionset")
    > lit('<')
    > ConText
    > lit(">")
    > -(lit('(') > +alpha > lit(')'))
    | seriesset
    ;

  BOOST_SPIRIT_DEFINE(ConText,
                      oneset, letterset, wordset, labelset,
                      weightset,
                      expressionset, seriesset);

  struct ConText_class : error_handler_base {};
}

namespace vcsn::dyn
{
  parser::ConText_type const& context_parser()
  {
    return parser::ConText;
  }
}

// context.cc
namespace vcsn::dyn::parser
{
  BOOST_SPIRIT_INSTANTIATE(ConText_type, iterator_type, parse_context_type);
}

namespace vcsn::ast
{
  std::string translate_context(const std::string& ctx)
  {
    // AST.
    auto res = vcsn::dyn::ast::ConText{};

    auto iter = ctx.cbegin();
    auto end = ctx.cend();

    // Our error handler
    using boost::spirit::x3::with;
    using vcsn::dyn::parser::error_handler_type;
    using vcsn::dyn::parser::error_handler_tag;
    auto error_handler = error_handler_type(iter, end, std::cerr, "input");

    // Our parser
    auto const parser =
      // we pass our error handler to the parser so we can access
      // it later on in our on_error and on_sucess handlers
      with<error_handler_tag>(std::ref(error_handler))
      [
        vcsn::dyn::context_parser()
      ];

    bool r = phrase_parse(iter, end, parser,
                          boost::spirit::x3::ascii::space, res);

    if (r)
      {
        if (iter == end)
          {
            auto o = std::ostringstream{};
            o << res;
            return o.str();
          }
        else
          {
            error_handler(iter, "Error! Expecting end of input here: ");
            vcsn::raise("failed1");
          }
      }
    else
      {
        vcsn::raise("failed2");
      }
  }
}
