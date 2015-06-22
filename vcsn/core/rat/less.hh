#pragma once

#include <boost/range/algorithm/lexicographical_compare.hpp>

#include <vcsn/core/rat/fwd.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/functional.hh> // vcsn::less

namespace vcsn
{

  namespace rat
  {

    template <typename ExpSet>
    class less
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
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
      bool
      operator()(expression_t lhs, expression_t rhs)
      {
        rat::size<ExpSet> sizer;
        size_t lhss = sizer(lhs);
        size_t rhss = sizer(rhs);

        if (lhss < rhss)
          return true;
        else if (lhss > rhss)
          return false;
        else if (lhs->type() < rhs->type())
          return true;
        else if (lhs->type() > rhs->type())
          return false;
        else
          {
            rhs_ = rhs;
            lhs->accept(*this);
            return res_;
          }
      }

    private:
      /*-----------------------------------------------------------.
      | Unary visit functions than bounces to their binary peers.  |
      `-----------------------------------------------------------*/

#define DEFINE(Type)                                                    \
      VCSN_RAT_VISIT(Type, lhs)                                         \
      {                                                                 \
        res_ = less_(lhs, *down_pointer_cast<const Type ## _t>(rhs_));  \
      }

      DEFINE(atom);
      DEFINE(complement);
      DEFINE(conjunction);
      DEFINE(ldiv);
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod);
      DEFINE(rweight);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(sum);
      DEFINE(transposition);
      DEFINE(zero);
#undef DEFINE

      /*-------------------------------------------------------.
      | Binary functions that compare two nodes of same type.  |
      `-------------------------------------------------------*/

      bool less_(const zero_t&, const zero_t&)
      {
        return false;
      }

      bool less_(const one_t&, const one_t&)
      {
        return false;
      }

      bool less_(const atom_t& lhs, const atom_t& rhs)
      {
        return labelset_t::less(lhs.value(), rhs.value());
      }

      template <rat::exp::type_t Type>
      bool less_(const variadic_t<Type>& lhs, const variadic_t<Type>& rhs)
      {
        using boost::range::lexicographical_compare;
        auto ls = lhs.size();
        auto rs = rhs.size();
        if (ls < rs)
          return true;
        else if (rs < ls)
          return false;
        else
          return lexicographical_compare(lhs, rhs,
                                         vcsn::less<expressionset_t>{});
      }

      template <rat::exp::type_t Type>
      bool less_(const unary_t<Type>& lhs, const unary_t<Type>& rhs)
      {
        return expressionset_t::less(lhs.sub(), rhs.sub());
      }

      template <rat::exp::type_t Type>
      bool less_(const weight_node_t<Type>& lhs, const weight_node_t<Type>& rhs)
      {
        // Lexicographic comparison on sub-expression, and then weight.
        if (expressionset_t::less(lhs.sub(), rhs.sub()))
          return true;
        else if (expressionset_t::less(rhs.sub(), lhs.sub()))
          return false;
        else
          return weightset_t::less(lhs.weight(), rhs.weight());
      }

   private:
      expression_t rhs_;
      bool res_;
    };
  }
}
