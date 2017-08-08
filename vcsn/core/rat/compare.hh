#pragma once

#include <vcsn/core/rat/fwd.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/functional.hh> // vcsn::lexicographical_cmp

namespace vcsn
{

  namespace rat
  {

    /// A functor for three-way comparison between two expressions.
    /// Implements the shortlex order.
    template <typename ExpSet>
    class compare
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using self_t = compare;
      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using super_t = typename expressionset_t::const_visitor;
      using node_t = typename super_t::node_t;
      using inner_t = typename super_t::inner_t;
      template <rat::exp::type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <rat::exp::type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      template <rat::exp::type_t Type>
      using weight_node_t = typename super_t::template weight_node_t<Type>;

      /// Whether \a lhs < \a rhs.
      ///
      /// Not just the entry point, it's also called at each recursion
      /// level.  In particular, that's where size and node types are
      /// considered, routines below handle the case of nodes of same
      /// sizes and same type.
      int operator()(expression_t lhs, expression_t rhs)
      {
        if (lhs == rhs)
          return 0;
        else
          {
            auto lhss = int(size<ExpSet>(lhs));
            auto rhss = int(size<ExpSet>(rhs));

            if (auto res = lhss - rhss)
              return res;
            else if (auto res = int(lhs->type()) - int(rhs->type()))
              return res;
            else
              {
                rhs_ = rhs;
                lhs->accept(*this);
                return res_;
              }
          }
      }

    private:
      /*-----------------------------------------------------------.
      | Unary visit functions than bounces to their binary peers.  |
      `-----------------------------------------------------------*/

#define DEFINE(Type)                                                    \
      VCSN_RAT_VISIT(Type, lhs)                                         \
      {                                                                 \
        res_ = cmp_(lhs, *down_pointer_cast<const Type ## _t>(rhs_));   \
      }

      DEFINE(add);
      DEFINE(atom);
      DEFINE(complement);
      DEFINE(compose);
      DEFINE(conjunction);
      DEFINE(infiltrate);
      DEFINE(ldivide);
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(mul);
      DEFINE(name);
      DEFINE(rweight);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(transposition);
      DEFINE(zero);
#undef DEFINE

      using tuple_t = typename super_t::tuple_t;

      template <typename = void>
      struct visit_tuple
      {
        using tupleset_t = typename expressionset_t::template as_tupleset_t<>;
        /// Entry point: down_cast rhs_ and bounce to binary operator().
        int operator()(const tuple_t& lhs)
        {
          return operator()(lhs,
                            *down_pointer_cast<const tuple_t>(self_.rhs_));
        }

        /// Binary operator().
        int operator()(const tuple_t& lhs, const tuple_t& rhs)
        {
          return tupleset_t::compare(lhs.sub(), rhs.sub());
        }

        const self_t& self_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        detail::static_if<context_t::is_lat>
          ([this](auto&& v)
           {
             res_ = visit_tuple<decltype(v)>{*this}(v);
           })
          (v);
      }

      /*-------------------------------------------------------.
      | Binary functions that compare two nodes of same type.  |
      `-------------------------------------------------------*/

      int cmp_(const zero_t&, const zero_t&)
      {
        return 0;
      }

      int cmp_(const one_t&, const one_t&)
      {
        return 0;
      }

      int cmp_(const atom_t& lhs, const atom_t& rhs)
      {
        return labelset_t::compare(lhs.value(), rhs.value());
      }

      int cmp_(const name_t& lhs, const name_t& rhs)
      {
        if (auto res = (*this)(lhs.sub(), rhs.sub()))
          return res;
        else
          return lhs.name_get().get().compare(rhs.name_get());
      }

      template <rat::exp::type_t Type>
      int cmp_(const variadic_t<Type>& lhs, const variadic_t<Type>& rhs)
      {
        if (auto res = int(lhs.size()) - int(rhs.size()))
          return res;
        else
          return lexicographical_cmp(lhs, rhs,
                                     vcsn::detail::compare<expressionset_t>{});
      }

      template <rat::exp::type_t Type>
      int cmp_(const unary_t<Type>& lhs, const unary_t<Type>& rhs)
      {
        return (*this)(lhs.sub(), rhs.sub());
      }

      template <rat::exp::type_t Type>
      int cmp_(const weight_node_t<Type>& lhs, const weight_node_t<Type>& rhs)
      {
        // Lexicographic comparison on sub-expression, and then weight.
        if (auto res = (*this)(lhs.sub(), rhs.sub()))
          return res;
        else
          return weightset_t::compare(lhs.weight(), rhs.weight());
      }

   private:
      /// The right-hand side expression with which the current node
      /// is compared.  Updated by the recursion.
      expression_t rhs_;
      /// The current result.
      int res_;
    };
  }
}
