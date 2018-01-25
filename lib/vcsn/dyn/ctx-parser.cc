#include <iostream>
#include <string>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
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

namespace
{
  // https://stackoverflow.com/questions/43089395
  template <typename T>
  struct as_type
  {
    template <typename Expr>
    auto operator[](Expr&& expr) const
    {
      namespace x3 = boost::spirit::x3;
      return x3::rule<struct _, T>{"as"}
      = x3::as_parser(std::forward<Expr>(expr));
    }
  };

  template <typename T>
  static const as_type<T> as = {};
}

// Public interface.
namespace vcsn::dyn::parser
{
  namespace x3 = boost::spirit::x3;
  namespace enc = x3::standard;

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
  using enc::char_;
  using enc::alnum;
  using enc::alpha;
  using enc::string;

  struct literal_weighsets_
    : x3::symbols<const char*>
  {
    literal_weighsets_()
    {
      add
        ("𝔹", "b")
        ("𝔽₂", "f2")
        ("Log", "log")
        ("ℕmin", "nmin")
        ("ℚ", "q")
        ("ℚmp", "qmp")
        ("ℝ", "r")
        ("ℝmin", "rmin")
        ("ℤ", "z")
        ("ℤmin", "zmin")
        ;
    }
  } literal_weighsets;

  // Rule IDS.
  struct x_class;
  struct one_class;
  struct letterset_class;
  struct wordset_class;
  struct labelset_class;
  struct labelset_basic_class;
  struct expressionset_class;
  struct polynomialset_class;
  struct seriesset_class;
  struct ConText_class;
  struct weightset_class;
  struct weightset_basic_class;

  // Rules.
  x3::rule<x_class> const
      x = "x";
  x3::rule<one_class, ast::oneset> const
      oneset = "oneset";
  x3::rule<letterset_class, ast::letterset> const
      letterset = "letterset";
  x3::rule<wordset_class, ast::wordset> const
      wordset = "wordset";
  x3::rule<labelset_basic_class, ast::labelset> const
      labelset_basic = "labelset_basic";
  x3::rule<labelset_class, ast::labelset> const
      labelset = "labelset";
  x3::rule<expressionset_class, ast::expressionset> const
      expressionset = "expressionset";
  x3::rule<polynomialset_class, ast::polynomialset> const
      polynomialset = "polynomialset";
  x3::rule<seriesset_class, ast::expressionset> const
      seriesset = "seriesset";
  x3::rule<ConText_class, ast::ConText> const
      ConText = "context";
  x3::rule<weightset_class, ast::weightset> const
      weightset = "weightset";
  x3::rule<weightset_basic_class, ast::weightset> const
      weightset_basic = "weightset_basic";

  auto mkseriesset = [](auto& ctx) {
    _val(ctx) = ast::expressionset{ast::ConText{_attr(ctx)}, "series"};
  };

  // Grammar.
  const auto ConText_def = eps
    > labelset
    > (lit("->") | lit(',') | lit("→"))
    > weightset
  ;

  const auto append = [](auto& ctx) {
    auto& val = _val(ctx);
    auto c = _attr(ctx);
    if (c == '(' || c == ')')
      val += '\\';
    val += c;
  };

  const auto escape = [](auto& ctx) {
    _val(ctx) += _attr(ctx);
  };

  const auto gen
    = (as<std::string>[char_('\\') >> char_])  [escape]
    | (char_ - ']')                            [append]
    ;

  const auto gens
    = lit("[...]")
    | as<std::string>[lit('[') >> lexeme[*gen] >> lit(']')]
    ;

  const auto gen_paren
    = (as<std::string>[char_('\\') >> char_])  [escape]
    | (char_ - ')')                            [append]
    ;

  // generators in parens, or nothing.
  const auto gens_parens
  = as<std::string>[lit('(') >> lexeme[*gen_paren] >> lit(')')]
    ;

  // <char>
  // <char(x)>
  // (x)
  const auto typed_gens
    = lit("<") >> (string("char") | string("string")) >> -gens_parens >> lit('>')
    | attr(std::string{"char"}) >> -gens_parens
    ;

  // <char>
  // eps
  const auto letter_type
    = lit("<") >> (string("char") | string("string")) >> lit('>')
    | attr(std::string{"char"})
    ;

  const auto oneset_def
    = lit("lao") >> !alnum
    ;

  // lal
  // [a], [a]?, <char>[a], <char>[a]?
  const auto letterset_def
    = lit("lal") >> typed_gens >> !alnum
    // Accept `[a]?` to facilitate transition: we print `lal(a), b`
    // as `[a]?  → 𝔹`.
    | letter_type >> gens >> -lit('?') >> !char_('*')
   ;

  // law
  // [a]*, <char>[a]*
  const auto wordset_def
    = lit("law") >> typed_gens >> !alnum
    | letter_type >> gens >> lit('*')
    ;

  const auto labelset_basic_def
    = oneset
    | letterset
    | wordset
    | lit("lat") > lit('<') > (labelset % ',') > lit('>')
    | expressionset
    | polynomialset
    ;

  // Cartesian product.
  const auto x_def
    = lit("×") | lit('x');

  // One or more labelsets.
  const auto labelset_def
    = as<ast::labelsets>[labelset_basic >> x > (labelset_basic % x)]
    | labelset_basic
    ;

  // An identifier must be separated from other alnum
  auto ident(const std::string& id)
  {
    // Declared as a `lexeme` so that we don't skip spaces.  Otherwise
    // `b x b` (standing for Cartesian product) would reject the first
    // `b` as it is followed (after skipping spaces) by an alnum.
    return lexeme[string(id) >> ! alnum];
  }

  const auto weightset_basic_def =
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
    | literal_weighsets
    | expressionset
    | polynomialset
    | lit("lat") > lit('<') > (weightset % ',') > lit('>')
    ;

  // One or more weightsets.
  const auto weightset_def
    = as<ast::weightsets>[weightset_basic >> x >> (weightset_basic % x)]
    | weightset_basic
    ;

  const auto seriesset_def =
    (lit("seriesset") > lit('<') > ConText > lit(">")) [mkseriesset]
    ;

  const auto expressionset_def
    = lit("expressionset") > '<'
    > ConText
    > lit('>')
    > -(lit('(') > +alpha > lit(')')) // identities.
    | lit("RatE") > '[' > ConText > ']' > -('(' > +alpha > ')') // identities.
    | seriesset
    ;

  const auto polynomialset_def
    = lit("polynomialset") > '<' > ConText > '>'
    | lit("Poly") > '[' > ConText > ']'
    ;

  BOOST_SPIRIT_DEFINE(x,
                      ConText,
                      oneset, letterset, wordset, labelset_basic, labelset,
                      weightset_basic, weightset,
                      expressionset, seriesset,
                      polynomialset);

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

    auto err = std::ostringstream{};
    auto error_handler = error_handler_type(iter, end, err, "");

    // Our parser
    auto const parser =
      // We pass our error handler to the parser so we can access
      // it later on in our on_error and on_sucess handlers.
      with<error_handler_tag>(std::ref(error_handler))
      [
        vcsn::dyn::context_parser()
      ];

    bool r = phrase_parse(iter, end, parser,
                          boost::spirit::x3::standard::space, res);

    if (r)
      {
        if (iter == end)
          {
            assert(err.str().empty());
            auto o = std::ostringstream{};
            o << res;
            return o.str();
          }
        else
          {
            error_handler(iter, "expecting end of input here: ");
            vcsn::raise(err.str());
          }
      }
    else
      {
        vcsn::raise(err.str());
      }
  }
}
