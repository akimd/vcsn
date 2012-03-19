#ifndef VCSN_CORE_RAT_RAT_EXP_HXX_
# define VCSN_CORE_RAT_RAT_EXP_HXX_

# include <algorithm>
# include <cassert>

# include <misc/cast.hh>
# include <core/rat/RatExpFactory.hh>
# include <core/rat/node.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    RatExpFactory<WeightSet>::RatExpFactory()
      : ws_(0)
    {}

    template <class WeightSet>
    RatExpFactory<WeightSet>::RatExpFactory(const WeightSet& ws)
      : ws_(&ws)
    {}

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_mul(RatExp* e)
    {
      return op_mul(down_cast<node_t*>(e));
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_mul(RatExp* l, RatExp* r)
    {
      auto left = down_cast<node_t*>(l);
      auto right = down_cast<node_t*>(r);
      return op_mul(left, right);
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_add(RatExp* l, RatExp* r)
    {
      auto left = down_cast<node_t*>(l);
      auto right = down_cast<node_t*>(r);
      return op_add(left, right);
    }

    template <class WeightSet>
    inline
    RatExp*
    RatExpFactory<WeightSet>::op_kleene(RatExp* e)
    {
      return op_kleene(down_cast<node_t*>(e));
    }

    // Implement function for concatenation operators.

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>*
    mul(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r)
    {
      auto res = new RatExpConcat<WeightSet>();
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
    auto
    RatExpFactory<WeightSet>::op_mul(node_t* e)
      -> node_t*
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
    auto
    RatExpFactory<WeightSet>::op_mul(node_t* l, node_t* r)
      -> node_t*
    {
      // Trivial Identity
      // E.0 = 0.E = 0
      // E.1 = 1.E = E
      if (node_t::ZERO == l->type() || node_t::ONE == r->type())
      {
        delete r;
        return l;
      }
      if (node_t::ZERO == r->type() || node_t::ONE == l->type())
      {
        delete l;
        return r;
      }
      // END: Trivial Identity

      if (node_t::CONCAT == l->type())
      {
        auto left = down_cast<RatExpConcat<WeightSet>*>(l);
        if (node_t::CONCAT == r->type())
        {
          auto right = down_cast<RatExpConcat<WeightSet>*>(r);
          return mul(left, right);
        }
        else
        {
          return mul(left, r);
        }
      }
      else if (node_t::CONCAT == r->type())
      {
        auto right = down_cast<RatExpConcat<WeightSet>*>(r);
        return mul(l, right);
      }
      else
      {
        return mul(l, r);
      }
    }

    template <class WeightSet>
    inline
    auto
    RatExpFactory<WeightSet>::op_add(node_t* l, node_t* r)
      -> node_t*
    {
      // Trivial Identity
      // E+0 = 0+E = E
      if (node_t::ZERO == l->type())
      {
        delete l;
        return r;
      }
      if (node_t::ZERO == r->type())
      {
        delete r;
        return l;
      }
      // END: Trivial Identity

      if (node_t::PLUS == l->type())
      {
        auto res = down_cast<RatExpPlus<WeightSet>*>(l);
        if (node_t::PLUS == r->type())
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
      else if (node_t::PLUS == r->type())
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
    auto
    RatExpFactory<WeightSet>::op_kleene(RatExpNode<WeightSet>* e)
      -> node_t*
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
      auto expr = down_cast<node_t*>(e);
      switch (expr->weight_type())
      {
      case node_t::L_WEIGHT:
      {
        auto lweight = down_cast<LWeightNode<WeightSet>*>(expr);
        return op_weight(lweight, w);
      }

      case node_t::LR_WEIGHT:
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
      auto expr = down_cast<node_t*>(e);
      switch (expr->weight_type())
      {
      case node_t::L_WEIGHT:
      {
        auto rweight = down_cast<LWeightNode<WeightSet>*>(expr);
        return op_weight(rweight, w);
      }
      case node_t::LR_WEIGHT:
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
    auto
    RatExpFactory<WeightSet>::op_weight(LWeightNode<WeightSet>* e,
                                        weight_str_container* w)
      -> node_t*
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
    auto
    RatExpFactory<WeightSet>::op_weight(weight_str_container* w,
                                        LRWeightNode<WeightSet>* e)
      -> node_t*
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
    auto
    RatExpFactory<WeightSet>::op_weight(LRWeightNode<WeightSet>* e,
                                        weight_str_container* w)
      -> node_t*
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

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_RAT_EXP_HXX_
