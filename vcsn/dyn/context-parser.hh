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
      /// Accept anything.
      std::shared_ptr<ast_node> any_();

      /// Return the next word in the stream.  Does not consider that
      /// underscore is word-constituent.
      std::string word_();

      /// A generator set (e.g., "char(abc)" or "char").
      std::shared_ptr<const genset> genset_();
      std::shared_ptr<const genset> genset_(const std::string& letter_type);

      /// `<LabelSet>, <WeightSet>`.
      std::shared_ptr<context> context_();
      std::shared_ptr<context> context_(const std::string& word);
      /// When the labelset was already parsed.
      std::shared_ptr<context> context_(const std::shared_ptr<ast_node>& ls);

      /// `<LabelSet>`.
      std::shared_ptr<ast_node> labelset_();
      std::shared_ptr<ast_node> labelset_(const std::string& kind);

      /// `<WeightSet>`.
      std::shared_ptr<ast_node> weightset_();
      std::shared_ptr<ast_node> weightset_(const std::string& ws);

      /// `<Automaton> "<" <Context> ">"`.
      std::shared_ptr<automaton> automaton_(std::string prefix);
      std::shared_ptr<tupleset> tupleset_();

      /// `"expressionset" "<" <Context> ">"`.
      std::shared_ptr<expressionset> expressionset_();
      /// No optional parameter.
      std::shared_ptr<expressionset> seriesset_();

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
          "lal_char",
          "lan",
          "lan_char",
          "lao",
          "law_char",
          "letterset",
          "nullableset",
          "wordset",
        };
    };
  }
}
