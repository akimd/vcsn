#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <vcsn/core/rat/identities.hh>

#include <lib/vcsn/dyn/context-visitor.hh>
#include <vcsn/dyn/fwd.hh>


namespace vcsn
{
  namespace ast
  {

#define ACCEPT()                                    \
      virtual void accept(context_visitor &v) const \
      {                                             \
        v.visit(*this);                             \
      }

    class ast_node
    {
    public:
      virtual ~ast_node() = default;

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


    class tuple: public ast_node
    {
    public:
      using value_t = std::vector<std::shared_ptr<ast_node>>;

      tuple(const value_t& sets)
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

    class oneset : public ast_node
    {
    public:
      oneset()
      {}

      ACCEPT()
      virtual bool has_one() const { return true; }
    };


    /// Represents the "alphabets", or "generator set".
    ///
    /// e.g. "char_letter(abc)".
    class genset: public ast_node
    {
    public:
      genset(const std::string& letter_type, const std::string& gens)
        : letter_type_(letter_type)
        , generators_(gens)
      {}

      ACCEPT()

      /// The char type.
      const std::string& letter_type() const
      {
        return letter_type_;
      }

      /// The generators.
      const std::string& generators() const
      {
        return generators_;
      }

    private:
      const std::string letter_type_;
      const std::string generators_;
    };

    /// Support for letterset<GenSet>.
    class letterset: public ast_node
    {
    public:
      letterset(const std::shared_ptr<const ast_node>& gs)
        : gs_(gs)
      {}

      /// The generator set.
      std::shared_ptr<const ast_node> genset() const
      {
        return gs_;
      }

      ACCEPT()
      virtual bool has_one() const { return false; }

    private:
      const std::shared_ptr<const ast_node> gs_;
    };


    /// Support for wordset<GenSet>.
    class wordset: public ast_node
    {
    public:
      wordset(const std::shared_ptr<const ast_node>& gs)
        : gs_(gs)
      {}

      /// The generator set.
      std::shared_ptr<const ast_node> genset() const
      {
        return gs_;
      }

      ACCEPT()
      virtual bool has_one() const { return true; }

    private:
      const std::shared_ptr<const ast_node> gs_;
    };

    class expressionset: public ast_node
    {
    public:
      expressionset(std::shared_ptr<context> ctx,
                rat::identities ids)
        : ctx_(ctx)
        , identities_(ids)
      {}

      const std::shared_ptr<context> get_context() const
      {
        return ctx_;
      }

      rat::identities get_identities() const
      {
        return identities_;
      }

      ACCEPT()
      virtual bool has_one() const { return true; }

    private:
      std::shared_ptr<context> ctx_;
      rat::identities identities_;
    };


    class expansionset: public ast_node
    {
    public:
      expansionset(std::shared_ptr<expressionset> rs)
        : rs_(rs)
      {}

      const std::shared_ptr<expressionset> get_expressionset() const
      {
        return rs_;
      }

      ACCEPT()
      virtual bool has_one() const { return true; }

    private:
      std::shared_ptr<expressionset> rs_;
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
#undef ACCEPT
  }
}
