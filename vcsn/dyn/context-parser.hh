#ifndef VCSN_DYN_CONTEXT_PARSER_HH
# define VCSN_DYN_CONTEXT_PARSER_HH

# include <sstream>
# include <string>

# include <vcsn/core/rat/identities.hh>
# include <vcsn/dyn/type-ast.hh>
# include <vcsn/misc/stream.hh>

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
      std::string word();

      /// List of letters.
      std::string alphabet_();

      /// `<LabelSet>_<WeightSet>`.
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

      /// `"ratexpset" "<" <Context> ">"`.
      std::shared_ptr<ratexpset> ratexpset_();
      std::shared_ptr<ratexpset> ratexpset_series_(); // No optional parameter

      /// `"polynomialset" "<" <Context> ">"`.
      std::shared_ptr<polynomialset> polynomialset_();

      /// `<LabelSet> | <WeightSet>`
      std::shared_ptr<ast_node> labelset_or_weightset_();
      std::shared_ptr<ast_node> labelset_or_weightset_(const std::string& kind);

      std::istringstream& is_;
    };
  }
}

#endif // !VCSN_DYN_CONTEXT_PARSER_HH
