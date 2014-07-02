#ifndef VCSN_DYN_TYPE_AST_HH
# define VCSN_DYN_TYPE_AST_HH

# include <iostream>
# include <memory>
# include <vector>

# include <vcsn/dyn/context-visitor.hh>
# include <vcsn/dyn/fwd.hh>


namespace vcsn
{
  namespace ast
  {

# define ACCEPT()                                   \
      virtual void accept(context_visitor &v) const \
      {                                             \
        v.visit(*this);                             \
      }

    class ast_node
    {
    public:
#ifndef COVERAGE
      virtual ~ast_node() = default;
#endif

      virtual void accept(context_visitor &v) const = 0;

      virtual bool has_one() const { return false; }
    };

    class context: public ast_node
    {
    public:
      context(std::shared_ptr<ast_node> ls, std::shared_ptr<ast_node> ws)
        : ls_(ls), ws_(ws)
      {}

      const std::shared_ptr<ast_node> get_labelset() const
      {
        return ls_;
      }

      const std::shared_ptr<ast_node> get_weightset() const
      {
        return ws_;
      }

      ACCEPT()

    private:
      std::shared_ptr<ast_node> ls_;
      std::shared_ptr<ast_node> ws_;
    };

    class tupleset: public ast_node
    {
    public:
      using value_t = std::vector<std::shared_ptr<ast_node>>;

      tupleset(const value_t& sets)
        : sets_(sets)
      {}

      const value_t get_sets() const
      {
        return sets_;
      }

      ACCEPT()
      virtual bool has_one() const
      {
        for (auto s : get_sets())
          if (!s->has_one())
            return false;
        return true;
      }

    private:
      value_t sets_;
    };

    class nullableset : public ast_node
    {
    public:
      nullableset(std::shared_ptr<ast_node> ls)
        : ls_(ls)
      {}

      const std::shared_ptr<ast_node> get_labelset() const
      {
        return ls_;
      }

      ACCEPT()

      virtual bool has_one() const { return true; }

    private:
      std::shared_ptr<ast_node> ls_;
    };

    class oneset : public ast_node
    {
    public:
      oneset()
      {}

      ACCEPT()
      virtual bool has_one() const { return true; }
    };

    class letterset: public ast_node
    {
    public:
      letterset(const std::string& alpha)
        : alpha_(alpha)
      {}

      const std::string& get_alpha() const
      {
        return alpha_;
      }

      ACCEPT()
      virtual bool has_one() const { return false; }
    private:
      const std::string alpha_;
    };

    class wordset: public ast_node
    {
    public:
      wordset(const std::string& alpha)
        : alpha_(alpha)
      {}

      const std::string& get_alpha() const
      {
        return alpha_;
      }

      ACCEPT()
      virtual bool has_one() const { return true; }
    private:
      const std::string alpha_;
    };

    class ratexpset: public ast_node
    {
    public:
      ratexpset(std::shared_ptr<context> ctx)
        : ctx_(ctx)
      {}

      const std::shared_ptr<context> get_context() const
      {
        return ctx_;
      }

      ACCEPT()
      virtual bool has_one() const { return true; }

    private:
      std::shared_ptr<context> ctx_;
    };

    class weightset: public ast_node
    {
    public:
      weightset(const std::string& type)
        : type_(type)
      {}

      const std::string& get_type() const
      {
        return type_;
      }

      ACCEPT()
    private:
      std::string type_;
    };

    class other : public ast_node
    {
    public:
      other(const std::string& type)
        : type_(type)
      {}

      const std::string& get_type() const
      {
        return type_;
      }

      ACCEPT()
    private:
      std::string type_;
    };

    class automaton: public ast_node
    {
    public:
      using node_t = std::shared_ptr<ast_node>;
      using nodes_t = std::vector<node_t>;
      automaton(const std::string& type, const node_t& child)
        : type_(type)
        , children_{child}
      {}

      automaton(const std::string& type, const nodes_t& children)
        : type_(type)
        , children_(children)
      {}

      const std::string& get_type() const
      {
        return type_;
      }

      const nodes_t& get_content() const
      {
        return children_;
      }

      nodes_t& get_content()
      {
        return children_;
      }

      ACCEPT()

    private:
      std::string type_;
      nodes_t children_;
    };

    class polynomialset : public ast_node
    {
    public:
      polynomialset(std::shared_ptr<ast_node> child)
        : child_(child)
      {}

      const std::shared_ptr<ast_node>& get_content() const
      {
        return child_;
      }

      ACCEPT()

    private:
        std::shared_ptr<ast_node> child_;
    };
# undef ACCEPT
  }
}



#endif // !VCSN_DYN_TYPE_AST_HH
