#pragma once

#include <sstream>
#include <string>

#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/type-ast.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  namespace ast
  {
    class context_parser
    {
    public:
      context_parser(std::istringstream& is)
        : is_(is)
      {}

      /// Accept anything.
      std::shared_ptr<ast_node> parse();

      /// Accept only a valid context.
      std::shared_ptr<ast_node> parse_context();

    private:
      /// Skip white spaces.
      void skip_space_();

      /// The next character, possibly preceded by spaces.
      int peek_();

      /// Accept this character, possibly preceded by spaces.
      char eat_(char c);

      /// Accept this string, possibly preceded by spaces.
      const std::string& eat_(const std::string& s);

      /// We managed to read \a res in \a is, check that `is_` is
      /// finished.
      void check_eof_(std::shared_ptr<ast_node> res);

      /// The next word in the stream.  Does not consider that
      /// underscore is word-constituent.  Skips spaces.
      std::string word_();

      /// The next parameters in the stream.  Must be right before the
      /// opening `<`.  Will return up to the matching `>`.  Skips
      /// spaces.
      std::string parameters_();

      /// Accept anything.
      std::shared_ptr<ast_node> any_();

      /// An optional generator set in brackets (e.g.,
      /// `<char_letters(abc)>` or `<char>`).  Defaults to `<char>` if
      /// the stream does not start with `<`.
      std::shared_ptr<const genset> genset_();

      /// A generator set (e.g., `char_letters(abc)` or `char`).
      /// \param letter_type should be `char_letter`, or `char`, etc.
      std::shared_ptr<const genset> genset_(std::string letter_type);

      /// `<LabelSet>, <WeightSet>`.
      std::shared_ptr<context> context_();
      std::shared_ptr<context> context_(std::string word);

      /// `<LabelSet>`.
      std::shared_ptr<ast_node> labelset_();
      std::shared_ptr<ast_node> labelset_(const std::string& kind);

      /// `<WeightSet>`.
      std::shared_ptr<ast_node> weightset_();
      std::shared_ptr<ast_node> weightset_(const std::string& ws);

      /// `<Automaton> "<" <Context> ">"`.
      std::shared_ptr<automaton> automaton_();
      std::shared_ptr<automaton> automaton_(std::string prefix);

      /// `"<" (<Any> ",")* ">"`.
      std::shared_ptr<tuple> tuple_();

      /// `"<" (<LabelSet> | <WeightSet> ",")+ ">"`.
      std::shared_ptr<tupleset> tupleset_();

      /// `"expressionset" "<" <Context> ">"`, possibly followed by identities.
      std::shared_ptr<expressionset> expressionset_();
      /// No optional argument.
      std::shared_ptr<expressionset> seriesset_();

      /// `"expansionset" "<" <Expressionset> ">"`.
      std::shared_ptr<expansionset> expansionset_();

      /// `"polynomialset" "<" <Context> ">"`.
      std::shared_ptr<polynomialset> polynomialset_();

      /// `<LabelSet> | <WeightSet>`
      std::shared_ptr<ast_node> labelset_or_weightset_();
      std::shared_ptr<ast_node> labelset_or_weightset_(const std::string& kind);
      /// The stream we are parsing.
      std::istringstream& is_;

      /// The set of terminal weightset names.
      std::set<std::string> weightsets_ =
        {
          "b",
          "f2",
          "log",
          "nmin",
          "q",
          "qmp",
          "r",
          "rmin",
          "z",
          "zmin",
        };

      /// The set of weightset names.
      std::set<std::string> labelsets_ =
        {
          "lal",
          "lal_char",
          "lan",
          "lan_char",
          "lao",
          "law",
          "law_char",
          "letterset",
          "nullableset",
          "wordset",
        };
    };
  }
}
