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
      using context_t = context_t_of<expressionset_t>;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_t = typename expressionset_t::const_visitor;
      using node_t = typename super_t::node_t;
      using inner_t = typename super_t::inner_t;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using weight_node_t = typename super_t::template weight_node_t<Type>;
      using leaf_t = typename super_t::leaf_t;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "hash"; }

      /// Entry point: return the hash of \a v.
      size_t
      operator()(const node_t& v)
      {
        res_ = 0;
        v.accept(*this);
        return res_;
      }

      /// Entry point: return the hash of \a v.
      size_t
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

      VCSN_RAT_VISIT(add, v)          { visit_variadic(v); }
      VCSN_RAT_VISIT(atom, v);
      VCSN_RAT_VISIT(complement, v)   { visit_unary(v); }
      VCSN_RAT_VISIT(compose, v)      { visit_variadic(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_variadic(v); }
      VCSN_RAT_VISIT(infiltrate, v)   { visit_variadic(v); }
      VCSN_RAT_VISIT(ldiv, v)         { visit_variadic(v); }
      VCSN_RAT_VISIT(lweight, v);
      VCSN_RAT_VISIT(one, v)          { visit_nullary(v); }
      VCSN_RAT_VISIT(prod, v)         { visit_variadic(v); }
      VCSN_RAT_VISIT(rweight, v);
      VCSN_RAT_VISIT(shuffle, v)      { visit_variadic(v); }
      VCSN_RAT_VISIT(star, v)         { visit_unary(v); }
      VCSN_RAT_VISIT(transposition, v){ visit_unary(v); }
      VCSN_RAT_VISIT(zero, v)         { visit_nullary(v); }

      using tuple_t = typename super_t::tuple_t;

      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        using tupleset_t = typename expressionset_t::template as_tupleset_t<>;
        size_t operator()(const tuple_t& v)
        {
          return tupleset_t::hash(v.sub());
        }
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        size_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        res_ = visit_tuple<>{}(v);
      }

      /// Update res_ by hashing the node type; this is needed for any node.
      void combine_type(const node_t& node);

      /// Traverse a nullary node (atom, `\z`, `\e`).
      void visit_nullary(const node_t& v);

      /// Traverse a unary node (`*`, `{c}`).
      template <rat::exp::type_t Type>
      void visit_unary(const unary_t<Type>& v);

      /// Traverse an n-ary node (`+`, concatenation, `&`, `:`).
      template <rat::exp::type_t Type>
      void visit_variadic(const variadic_t<Type>& v);

      /// Traverse a weight node (lweight, rweight).
      template <rat::exp::type_t Type>
      void visit_weight_node(const weight_node_t<Type>& v);

      size_t res_;
    };
  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/hash.hxx>
