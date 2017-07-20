#pragma once

#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
    template <typename ExpSet>
    class hash
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using self_t = hash;
      using context_t = context_t_of<expressionset_t>;
      using super_t = typename expressionset_t::const_visitor;

      /// Actual node, without indirection.
      using node_t = typename super_t::node_t;
      /// A shared_ptr to node_t.
      using expression_t = typename node_t::value_t;

      template <type_t Type>
      using constant_t = typename super_t::template constant_t<Type>;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      template <type_t Type>
      using weight_node_t = typename super_t::template weight_node_t<Type>;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "hash"; }

      /// Entry point: return the hash of \a v.
      size_t operator()(const expression_t& v)
      {
        res_ = 0;
        v->accept(*this);
        return res_;
      }

    private:

      /// Update res_ with the hash.
      void combine_(size_t h)
      {
        hash_combine(res_, h);
      }

      /// Update res_ by hashing the node type.  Must be called for
      /// every node.
      void combine_type_(const node_t& node)
      {
        combine_(int(node.type()));
      }

      VCSN_RAT_VISIT(add, v)          { visit_(v); }
      VCSN_RAT_VISIT(complement, v)   { visit_(v); }
      VCSN_RAT_VISIT(compose, v)      { visit_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_(v); }
      VCSN_RAT_VISIT(infiltrate, v)   { visit_(v); }
      VCSN_RAT_VISIT(ldivide, v)      { visit_(v); }
      VCSN_RAT_VISIT(lweight, v)      { visit_(v); }
      VCSN_RAT_VISIT(mul, v)          { visit_(v); }
      VCSN_RAT_VISIT(one, v)          { visit_(v); }
      VCSN_RAT_VISIT(rweight, v)      { visit_(v); }
      VCSN_RAT_VISIT(shuffle, v)      { visit_(v); }
      VCSN_RAT_VISIT(star, v)         { visit_(v); }
      VCSN_RAT_VISIT(transposition, v){ visit_(v); }
      VCSN_RAT_VISIT(zero, v)         { visit_(v); }

      VCSN_RAT_VISIT(atom, v)
      {
        combine_type_(v);
        combine_(ExpSet::labelset_t::hash(v.value()));
      }

      VCSN_RAT_VISIT(name, v)
      {
        combine_type_(v);
        combine_(hash_value(v.name_get()));
      }

      using tuple_t = typename super_t::tuple_t;

      template <typename = void>
      struct visit_tuple
      {
        using tupleset_t = typename expressionset_t::template as_tupleset_t<>;
        size_t operator()(const tuple_t& v)
        {
          return tupleset_t::hash(v.sub());
        }
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        detail::static_if<context_t::is_lat>
          ([this](auto&& v)
           {
             combine_type_(v);
             combine_(visit_tuple<decltype(v)>{}(v));
           })
          (v);
      }

      /// Traverse a nullary node.
      template <rat::exp::type_t Type>
      void visit_(const constant_t<Type>& v)
      {
        combine_type_(v);
      }

      /// Traverse a unary node.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        combine_type_(v);
        v.sub()->accept(*this);
      }

      /// Traverse an n-ary node.
      template <rat::exp::type_t Type>
      void visit_(const variadic_t<Type>& v)
      {
        combine_type_(v);
        for (const auto& child : v)
          child->accept(*this);
      }

      /// Traverse a weight node (lweight, rweight).
      template <rat::exp::type_t Type>
      void visit_(const weight_node_t<Type>& v)
      {
        combine_type_(v);
        combine_(ExpSet::weightset_t::hash(v.weight()));
        v.sub()->accept(*this);
      }

      /// The result, which must be updated incrementally.  Do not
      /// modify directly, call `combine_` instead.
      size_t res_;
    };
  } // namespace rat
} // namespace vcsn
