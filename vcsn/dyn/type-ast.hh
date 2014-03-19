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

# define IS_ONE()                                   \
      virtual bool is_one() const

# define ACCEPT()                                   \
      virtual void accept(context_visitor &v) const \
      {                                             \
        v.visit(*this);                             \
      }

    class ast_node
    {
    public:
      virtual ~ast_node()
      {}

      virtual void accept(context_visitor &v) const = 0;

      IS_ONE() { return false; }
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
      IS_ONE()
      {
        for (auto s : get_sets())
          if (!s->is_one())
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

      IS_ONE() { return true; }

    private:
      std::shared_ptr<ast_node> ls_;
    };

    class oneset : public ast_node
    {
    public:
      oneset()
      {}

      ACCEPT()
      IS_ONE() { return true; }
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
      IS_ONE() { return false; }
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
      IS_ONE() { return true; }
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
      IS_ONE() { return true; }

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
      automaton(const std::string& type, std::shared_ptr<ast_node> child)
        : type_(type), child_(child)
      {}

      const std::string& get_type() const
      {
        return type_;
      }

      const std::shared_ptr<ast_node>& get_content() const
      {
        return child_;
      }

      ACCEPT()

    private:
      std::string type_;
      std::shared_ptr<ast_node> child_;
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
# undef IS_ONE
  }
}



#endif /* !VCSN_DYN_TYPE_AST_HH */
