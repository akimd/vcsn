#ifndef VCSN_DYN_PARSER_HH
# define VCSN_DYN_PARSER_HH

# include <sstream>
# include <string>

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

      std::shared_ptr<ast_node> parse();

    private:
      std::shared_ptr<ast_node> parse_();
      std::string word();
      std::string get_alpha();
      std::shared_ptr<context> make_context();
      std::shared_ptr<context> make_context(const std::string& word);
      std::shared_ptr<ast_node> make_labelset();
      std::shared_ptr<ast_node> make_labelset(const std::string& kind);
      std::shared_ptr<ast_node> make_weightset();
      std::shared_ptr<ast_node> make_weightset(const std::string& ws);
      std::shared_ptr<automaton> make_automaton(const std::string& prefix);
      std::shared_ptr<tupleset> make_tupleset();
      std::shared_ptr<ratexpset> make_ratexpset();
      std::shared_ptr<ast_node> make_labelset_or_weightset();
      std::shared_ptr<ast_node>
        make_labelset_or_weightset(const std::string& kind);

      std::istringstream& is_;
    };
  }
}

#endif /* !VCSN_DYN_PARSER_HH */
