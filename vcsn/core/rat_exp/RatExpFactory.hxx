#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HXX_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HXX_

# include <algorithm>
# include <cassert>

# include <misc/cast.hh>
# include <core/rat_exp/RatExpFactory.hh>
# include <core/rat_exp/node.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    RatExpFactory<WeightSet>::RatExpFactory()
      : ws_(0)
    { }

    template <class WeightSet>
    RatExpFactory<WeightSet>::RatExpFactory(const WeightSet& ws)
      : ws_(&ws)
    { }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_mul(RatExp* e)
    {
      auto expr = down_cast<RatExpNode<WeightSet>*>(e);
      return op_mul(expr);
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_mul(RatExp* l, RatExp* r)
    {
      auto left = down_cast<RatExpNode<WeightSet>*>(l);
      auto right = down_cast<RatExpNode<WeightSet>*>(r);
      return op_mul(left, right);
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_add(RatExp* l, RatExp* r)
    {
      l = clean_node(l);
      r = clean_node(r);
      auto left = down_cast<RatExpNode<WeightSet>*>(l);
      auto right = down_cast<RatExpNode<WeightSet>*>(r);
      return op_add(left, right);
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_kleene(RatExp* e)
    {
      auto expr = down_cast<RatExpNode<WeightSet>*>(e);
      return op_kleene(expr);
    }

    // Implement function for concatenation operators.

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>*
    mul(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      RatExpConcat<WeightSet>* res = new RatExpConcat<WeightSet>();
      res->push_back(l);
      res->push_back(r);
      return res;
    }

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>*
    mul(RatExpConcat<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      l->push_back(r);
      return l;
    }

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>*
    mul(RatExpNode<WeightSet>* l, RatExpConcat<WeightSet>* r)
    {
      r->push_front(l);
      return r;
    }

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>*
    mul(RatExpConcat<WeightSet>* l, RatExpConcat<WeightSet>* r)
    {
      for (auto i : *r)
        l->push_back(i);
      return l;
    }

    // End

    template <class WeightSet>
    inline
    RatExpNode<WeightSet>*
    RatExpFactory<WeightSet>::op_mul(RatExpNode<WeightSet>* e)
    {
      return e;

      // if (RatExpConcat<WeightSet>::CONCAT == e->type())
      // {
      auto res = down_cast<RatExpConcat<WeightSet>*>(e);
      //   assert(res);
      //   return res;
      // }
      // else
      // {
      //   RatExpConcat<WeightSet>*res = new RatExpConcat<WeightSet>();
      //   res->push_front(e);
      //   return res;
      // }
    }

    template <class WeightSet>
    inline
    RatExpNode<WeightSet>*
    RatExpFactory<WeightSet>::op_mul(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      // Trivial Identity
      // E.0 = 0.E = 0
      // E.1 = 1.E = E
      if (RatExpNode<WeightSet>::ZERO == l->type()
          || RatExpNode<WeightSet>::ONE == r->type())
      {
        delete r;
        return l;
      }
      if (RatExpNode<WeightSet>::ZERO == r->type()
          || RatExpNode<WeightSet>::ONE == l->type())
      {
        delete l;
        return r;
      }
      // END: Trivial Identity

      if (RatExpNode<WeightSet>::CONCAT == l->type())
      {
        auto left = down_cast<RatExpConcat<WeightSet>*>(l);
        if (RatExpNode<WeightSet>::CONCAT == r->type())
        {
          auto right = down_cast<RatExpConcat<WeightSet>*>(r);
          return mul<WeightSet>(left, right);
        }
        else
        {
          return mul<WeightSet>(left, r);
        }
      }
      else if (RatExpNode<WeightSet>::CONCAT == r->type())
      {
        auto right = down_cast<RatExpConcat<WeightSet>*>(r);
        return mul<WeightSet>(l, right);
      }
      else
      {
        return mul<WeightSet>(l, r);
      }
    }

    template <class WeightSet>
    inline
    RatExpNode<WeightSet>*
    RatExpFactory<WeightSet>::op_add(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      // Trivial Identity
      // E+0 = 0+E = E
      if (RatExpNode<WeightSet>::ZERO == l->type())
      {
        delete l;
        return r;
      }
      if (RatExpNode<WeightSet>::ZERO == r->type())
      {
        delete r;
        return l;
      }
      // END: Trivial Identity

      if (RatExpNode<WeightSet>::PLUS == l->type())
      {
        auto res = down_cast<RatExpPlus<WeightSet>*>(l);
        if (RatExpNode<WeightSet>::PLUS == r->type())
        {
          auto right = down_cast<RatExpPlus<WeightSet>*>(r);
          for (auto it : *right)
            res->push_back(it);
        }
        else
        {
          res->push_back(r);
        }
        return res;
      }
      else if (RatExpNode<WeightSet>::PLUS == r->type())
      {
        auto res = down_cast<RatExpPlus<WeightSet>*>(r);
        res->push_front(l);
        return res;
      }
      else
      {
        RatExpPlus<WeightSet>* res = new RatExpPlus<WeightSet>();
        res->push_front(r);
        res->push_front(l);
        return res;
      }
    }

    template <class WeightSet>
    inline
    RatExpKleene<WeightSet>*
    RatExpFactory<WeightSet>::op_kleene(RatExpNode<WeightSet>* e)
    {
      if (RatExpNode<WeightSet>::ZERO == e->type())
      {
        // Trivial identity
        // (0)* == 1
        delete e;
        return new RatExpOne<WeightSet>;
      }
      else
        return new RatExpKleene<WeightSet>(e);
    }

    template <class WeightSet>
    inline
    RatExpOne<WeightSet> *
    RatExpFactory<WeightSet>::op_one()
    {
      return new RatExpOne<WeightSet>();
    }

    template <class WeightSet>
    inline
    RatExpZero<WeightSet>*
    RatExpFactory<WeightSet>::op_zero()
    {
      return new RatExpZero<WeightSet>();
    }

    template <class WeightSet>
    inline
    RatExpWord<WeightSet>*
    RatExpFactory<WeightSet>::op_word(std::string* w)
    {
      if (ws_ != nullptr)
        return new RatExpWord<WeightSet>(w, *ws_);
      else
        return new RatExpWord<WeightSet>(w);
    }

    template <class WeightSet>
    inline
    typename RatExpFactory<WeightSet>::weight_str_container*
    RatExpFactory<WeightSet>::op_weight(std::string* w)
    {
      weight_str_container* res = new weight_str_container();
      res->push_front(w);
      return res;
    }

    template <class WeightSet>
    RatExp*
    RatExpFactory<WeightSet>::op_weight(weight_str_container* w, RatExp* e)
    {
      if (!w)
        return e;
      // if w
      auto expr = down_cast<RatExpNode<WeightSet>*>(e);
      switch (expr->get_weight_type())
      {
      case RatExpNode<WeightSet>::L_WEIGHT:
      {
        auto lweight = down_cast<LWeightNode<WeightSet>*>(expr);
        return op_weight(lweight, w);
      }

      case RatExpNode<WeightSet>::LR_WEIGHT:
      {
        auto lrweight = down_cast<LRWeightNode<WeightSet>*>(expr);
        return op_weight(w, lrweight);
      }

      default:
        assert(false);
        return 0;
      }
    }

    template <class WeightSet>
    RatExp*
    RatExpFactory<WeightSet>::op_weight(RatExp* e, weight_str_container* w)
    {
      if (!w)
        return e;
      // if w
      auto expr = down_cast<RatExpNode<WeightSet>*>(e);
      switch (expr->get_weight_type())
      {
      case RatExpNode<WeightSet>::L_WEIGHT:
      {
        auto rweight = down_cast<LWeightNode<WeightSet>*>(expr);
        return op_weight(rweight, w);
      }
      case RatExpNode<WeightSet>::LR_WEIGHT:
      {
        auto rweight = down_cast<LRWeightNode<WeightSet>*>(expr);
        return op_weight(rweight, w);
      }
      default:
        assert(false);
        return 0;
      }
    }

    template<class WeightSet>
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_weight(LWeightNode<WeightSet>* e, weight_str_container* w)
    {
      for (auto i : *w)
      {
        weight_t new_weight = ws_->op_conv(*i);
        if (ws_->is_zero(new_weight))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        e->left_weight() = ws_->mul(e->left_weight(), new_weight);
      }
      return e;
    }

    template<class WeightSet>
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_weight(weight_str_container* w, LRWeightNode<WeightSet>* e)
    {
      for (auto i : *w)
      {
        weight_t new_weight = ws_->op_conv(*i);
        if (ws_->is_zero(new_weight))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        e->left_weight() = ws_->mul(e->left_weight(), new_weight);
      }
      return e;
    }

    template<class WeightSet>
    RatExpNode<WeightSet>*
    RatExpFactory<WeightSet>::op_weight(LRWeightNode<WeightSet>* e, weight_str_container* w)
    {
      for (auto i : *w)
      {
        weight_t new_weight = ws_->op_conv(*i);
        if (ws_->is_zero(new_weight))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        e->right_weight() = ws_->mul(e->right_weight(), new_weight);
      }
      return e;
    }

    template <class WeightSet>
    inline
    RatExpFactory<WeightSet>::op_weight(weight_type* w, weights_type* l)
    typename RatExpFactory<WeightSet>::weight_str_container*
    RatExpFactory<WeightSet>::op_weight(weight_str* w, weight_str_container* l)
    {
      l->push_front(w);
      return l;
    }

    template <class WeightSet>
    inline
    typename RatExpFactory<WeightSet>::weight_str_container*
    RatExpFactory<WeightSet>::op_weight(weight_str_container* l, weight_str* w)
    {
      l->push_front(w);
      return l;
    }

    template<class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::clean_node(RatExp* e)
    {
      auto expr = down_cast<RatExpNode<WeightSet>*>(e);

      switch (expr->type())
      {
      case RatExpNode<WeightSet>::PLUS:
      {
        auto plus = down_cast<RatExpPlus<WeightSet>*>(expr);
        if (1 == plus->size())
        {
          expr = *plus->begin();
          plus->erase(plus->begin());
          delete plus;
        }
        break;
      }
      case RatExpNode<WeightSet>::CONCAT:
      {
        auto concat = down_cast<RatExpConcat<WeightSet>*>(expr);
        if (1 == concat->size())
        {
          expr = *concat->begin();
          concat->erase(concat->begin());
          delete concat;
        }
        break;
      }
      default:
        break;
      }
      return expr;
    }

  } // namespace rat_exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HXX_
