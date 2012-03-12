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

    template<class WeightSet>
    inline
    RatExp *
    RatExpFactory<WeightSet>::op_mul(RatExp* e)
    {
      RatExpNode<WeightSet> *expr = down_cast<RatExpNode<WeightSet> *>(e);
      assert(expr);
      return op_mul(expr);
    }

    template<class WeightSet>
    inline
    RatExp *
    RatExpFactory<WeightSet>::op_mul(RatExp* l, RatExp* r)
    {
      RatExpNode<WeightSet> *left = down_cast<RatExpNode<WeightSet> *>(l);
      assert(left);
      RatExpNode<WeightSet> *right = down_cast<RatExpNode<WeightSet> *>(r);
      assert(right);
      return op_mul(left, right);
    }

    template<class WeightSet>
    inline
    RatExp *
    RatExpFactory<WeightSet>::op_add(RatExp* l, RatExp* r)
    {
      l = clean_node(l);
      r = clean_node(r);
      RatExpNode<WeightSet>* left = down_cast<RatExpNode<WeightSet> *>(l);
      assert(left);
      RatExpNode<WeightSet>* right = down_cast<RatExpNode<WeightSet> *>(r);
      assert(right);
      return op_add(left, right);
    }

    template<class WeightSet>
    inline
    RatExp *
    RatExpFactory<WeightSet>::op_kleene(RatExp* e)
    {
      RatExpNode<WeightSet>* expr = down_cast<RatExpNode<WeightSet> *>(e);
      assert(expr);
      return op_kleene(expr);
    }

    // Implement function for concatenation operators.

    template<class WeightSet>
    inline
    RatExpConcat<WeightSet> *
    mul(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      RatExpConcat<WeightSet>* res = new RatExpConcat<WeightSet>();
      res->push_back(l);
      res->push_back(r);
      return res;
    }

    template<class WeightSet>
    inline
    RatExpConcat<WeightSet> *
    mul(RatExpConcat<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      l->push_back(r);
      return l;
    }

    template<class WeightSet>
    inline
    RatExpConcat<WeightSet> *
    mul(RatExpNode<WeightSet>* l, RatExpConcat<WeightSet>* r)
    {
      r->push_front(l);
      return r;
    }

    template<class WeightSet>
    inline
    RatExpConcat<WeightSet> *
    mul(RatExpConcat<WeightSet>* l, RatExpConcat<WeightSet>* r)
    {
      for (auto i : *r)
        l->push_back(i);
      return l;
    }

    // End

    template<class WeightSet>
    inline
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_mul(RatExpNode<WeightSet>* e)
    {
      return e;

      // if (RatExpConcat<WeightSet>::CONCAT == e->get_type())
      // {
      //   RatExpConcat<WeightSet> *res = down_cast<RatExpConcat<WeightSet> *>(e);
      //   assert(res);
      //   return res;
      // }
      // else
      // {
      //   RatExpConcat<WeightSet> *res = new RatExpConcat<WeightSet>();
      //   res->push_front(e);
      //   return res;
      // }
    }

    template<class WeightSet>
    inline
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_mul(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      // Trivial Identity
      // E.0 = 0.E = 0
      // E.1 = 1.E = E
      if(RatExpNode<WeightSet>::ZERO == l->get_type()
         || (RatExpNode<WeightSet>::ONE == r->get_type()))
      {
        delete r;
        return l;
      }
      if(RatExpNode<WeightSet>::ZERO == r->get_type()
         || (RatExpNode<WeightSet>::ONE == l->get_type()))
      {
        delete l;
        return r;
      }
      // END: Trivial Identity

      if (RatExpNode<WeightSet>::CONCAT == l->get_type())
      {
        RatExpConcat<WeightSet> *left = down_cast<RatExpConcat<WeightSet> *>(l);
        assert(left);
        if (RatExpNode<WeightSet>::CONCAT == r->get_type())
        {
          RatExpConcat<WeightSet> *right = down_cast<RatExpConcat<WeightSet> *>(r);
          assert(right);
          return mul<WeightSet>(left, right);
        }
        else
        {
          return mul<WeightSet>(left, r);
        }
      }
      else if (RatExpNode<WeightSet>::CONCAT == r->get_type())
      {
        RatExpConcat<WeightSet> *right = down_cast<RatExpConcat<WeightSet> *>(r);
        assert(right);
        return mul<WeightSet>(l, right);
      }
      else
      {
        return mul<WeightSet>(l, r);
      }
    }

    template<class WeightSet>
    inline
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_add(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      // Trivial Identity
      // E+0 = 0+E = E
      if(RatExpNode<WeightSet>::ZERO == l->get_type())
      {
        delete l;
        return r;
      }
      if(RatExpNode<WeightSet>::ZERO == r->get_type())
      {
        delete r;
        return l;
      }
      // END: Trivial Identity

      if (RatExpNode<WeightSet>::PLUS == l->get_type())
      {
        RatExpPlus<WeightSet>* res = down_cast<RatExpPlus<WeightSet> *>(l);
        assert(res);
        if (RatExpNode<WeightSet>::PLUS == r->get_type())
        {
          RatExpPlus<WeightSet>* right = down_cast<RatExpPlus<WeightSet> *>(r);
          assert(right);
          for (auto * it : *right)
            res->push_back(it);
        }
        else
        {
          res->push_back(r);
        }
        return res;
      }
      else if (RatExpNode<WeightSet>::PLUS == r->get_type())
      {
        RatExpPlus<WeightSet>* res = down_cast<RatExpPlus<WeightSet> *>(r);
        assert(res);
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

    template<class WeightSet>
    inline
    RatExpKleene<WeightSet> *
    RatExpFactory<WeightSet>::op_kleene(RatExpNode<WeightSet>* e)
    {
      return new RatExpKleene<WeightSet>(e);
    }

    template<class WeightSet>
    inline
    RatExpOne<WeightSet>  *
    RatExpFactory<WeightSet>::op_one()
    {
      return new RatExpOne<WeightSet>();
    }

    template<class WeightSet>
    inline
    RatExpZero<WeightSet> *
    RatExpFactory<WeightSet>::op_zero()
    {
      return new RatExpZero<WeightSet>();
    }

    template<class WeightSet>
    inline
    RatExpWord<WeightSet> *
    RatExpFactory<WeightSet>::op_word(std::string *w)
    {
      return new RatExpWord<WeightSet>(w);
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExpFactory<WeightSet>::op_weight(weight_type* w)
    {
      weights_type* res = new weights_type();
      res->push_front(w);
      return res;
    }

    template<class WeightSet>
    RatExp *
    RatExpFactory<WeightSet>::op_weight(weights_type* w, RatExp* e)
    {
      if (!w)
        return e;
      // if w
      RatExpNode<WeightSet>* expr = down_cast<RatExpNode<WeightSet> *>(e);
      assert(expr);
      switch (expr->get_weight_type())
      {
      case RatExpNode<WeightSet>::L_WEIGHT:
      {
        LWeightNode<WeightSet>* lweight = down_cast<LWeightNode<WeightSet> *>(expr);
        assert(lweight);
        return op_weight(lweight, w);
      }

      case RatExpNode<WeightSet>::LR_WEIGHT:
      {
        LRWeightNode<WeightSet>* lrweight = down_cast<LRWeightNode<WeightSet> *>(expr);
        assert(lrweight);
        return op_weight(w, lrweight);
      }

      default:
        assert(false);
        return 0;
      }
    }

    template<class WeightSet>
    RatExp *
    RatExpFactory<WeightSet>::op_weight(RatExp* e, weights_type* w)
    {
      if (!w)
        return e;
      // if w
      RatExpNode<WeightSet> *expr = down_cast<RatExpNode<WeightSet> *>(e);
      assert(expr);
      switch (expr->get_weight_type())
      {
      case RatExpNode<WeightSet>::L_WEIGHT:
      {
        LWeightNode<WeightSet>* rweight = down_cast<LWeightNode<WeightSet> *>(expr);
          assert(rweight);
          return op_weight(rweight, w);
      }
      case RatExpNode<WeightSet>::LR_WEIGHT:
      {
        LRWeightNode<WeightSet>* rweight = down_cast<LRWeightNode<WeightSet> *>(expr);
        assert(rweight);
        return op_weight(rweight, w);
      }
      default:
        assert(false);
        return 0;
      }
    }

    template<class WeightSet>
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_weight(LWeightNode<WeightSet>* e, weights_type* w)
    {
      for (auto i : *w)
      {
        if(WeightSet::is_zero(WeightSet::op_conv(i)))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        WeightSet::op_mul_eq(e->left_weight(), i);
      }
      return e;
    }

    template<class WeightSet>
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_weight(weights_type* w, LRWeightNode<WeightSet>* e)
    {
      for (auto i : *w)
      {
        if(WeightSet::is_zero(WeightSet::op_conv(i)))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        WeightSet::op_mul_eq(e->left_weight(), i);
      }
      return e;
    }

    template<class WeightSet>
    RatExpNode<WeightSet> *
    RatExpFactory<WeightSet>::op_weight(LRWeightNode<WeightSet>* e, weights_type* w)
    {
      for (auto i : *w)
      {
        if(WeightSet::is_zero(WeightSet::op_conv(i)))
        {
          delete e;
          return new RatExpZero<WeightSet>();
        }
        WeightSet::op_mul_eq(e->right_weight(), i);
      }
      return e;
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExpFactory<WeightSet>::op_weight(weight_type* w, weights_type* l)
    {
      l->push_front(w);
      return l;
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExpFactory<WeightSet>::op_weight(weights_type* l, weight_type* w)
    {
      l->push_front(w);
      return l;
    }

    template<class WeightSet>
    inline
    RatExp *
    RatExpFactory<WeightSet>::clean_node(RatExp* e)
    {
      RatExpNode<WeightSet>* expr = down_cast<RatExpNode<WeightSet> *>(e);
      assert(expr);

      switch (expr->get_type())
      {
      case RatExpNode<WeightSet>::PLUS:
      {
        RatExpPlus<WeightSet>* plus = down_cast<RatExpPlus<WeightSet> *>(expr);
        assert(plus);
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
        RatExpConcat<WeightSet>* concat = down_cast<RatExpConcat<WeightSet> *>(expr);
        assert(concat);
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

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HXX_
