#ifndef VCSN_CORE_RAT_RAT_EXP_HXX_
# define VCSN_CORE_RAT_RAT_EXP_HXX_

# include <algorithm>
# include <cassert>

# include <misc/cast.hh>
# include <core/rat/factory.hh>
# include <core/rat/node.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    factory<WeightSet>::factory()
      : ws_(0)
    {}

    template <class WeightSet>
    factory<WeightSet>::factory(const WeightSet& ws)
      : ws_(&ws)
    {}

    template <class WeightSet>
    inline
    exp*
    factory<WeightSet>::op_mul(exp* e)
    {
      return op_mul(down_cast<node_t*>(e));
    }

    template <class WeightSet>
    inline
    exp*
    factory<WeightSet>::op_mul(exp* l, exp* r)
    {
      auto left = down_cast<node_t*>(l);
      auto right = down_cast<node_t*>(r);
      return op_mul(left, right);
    }

    template <class WeightSet>
    inline
    exp*
    factory<WeightSet>::op_add(exp* l, exp* r)
    {
      auto left = down_cast<node_t*>(l);
      auto right = down_cast<node_t*>(r);
      return op_add(left, right);
    }

    template <class WeightSet>
    inline
    exp*
    factory<WeightSet>::op_kleene(exp* e)
    {
      return op_kleene(down_cast<node_t*>(e));
    }

    // Implement function for concatenation operators.

    template <class WeightSet>
    inline
    concat<WeightSet>*
    mul(node<WeightSet>* l, node<WeightSet>* r)
    {
      auto res = new concat<WeightSet>();
      res->push_back(l);
      res->push_back(r);
      return res;
    }

    template <class WeightSet>
    inline
    concat<WeightSet>*
    mul(concat<WeightSet>* l, node<WeightSet>* r)
    {
      l->push_back(r);
      return l;
    }

    template <class WeightSet>
    inline
    concat<WeightSet>*
    mul(node<WeightSet>* l, concat<WeightSet>* r)
    {
      r->push_front(l);
      return r;
    }

    template <class WeightSet>
    inline
    concat<WeightSet>*
    mul(concat<WeightSet>* l, concat<WeightSet>* r)
    {
      for (auto i : *r)
        l->push_back(i);
      return l;
    }

    // End

    template <class WeightSet>
    inline
    auto
    factory<WeightSet>::op_mul(node_t* e)
      -> node_t*
    {
      return e;

      // if (concat<WeightSet>::CONCAT == e->type())
      // {
      auto res = down_cast<concat<WeightSet>*>(e);
      //   assert(res);
      //   return res;
      // }
      // else
      // {
      //   concat<WeightSet>*res = new concat<WeightSet>();
      //   res->push_front(e);
      //   return res;
      // }
    }

    template <class WeightSet>
    inline
    auto
    factory<WeightSet>::op_mul(node_t* l, node_t* r)
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
          auto left = down_cast<concat<WeightSet>*>(l);
          if (node_t::CONCAT == r->type())
            {
              auto right = down_cast<concat<WeightSet>*>(r);
              return mul(left, right);
            }
          else
            {
              return mul(left, r);
            }
        }
      else if (node_t::CONCAT == r->type())
        {
          auto right = down_cast<concat<WeightSet>*>(r);
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
    factory<WeightSet>::op_add(node_t* l, node_t* r)
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
          auto res = down_cast<plus<WeightSet>*>(l);
          if (node_t::PLUS == r->type())
            {
              auto right = down_cast<plus<WeightSet>*>(r);
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
          auto res = down_cast<plus<WeightSet>*>(r);
          res->push_front(l);
          return res;
        }
      else
        {
          plus<WeightSet>* res = new plus<WeightSet>();
          res->push_front(r);
          res->push_front(l);
          return res;
        }
    }

    template <class WeightSet>
    inline
    auto
    factory<WeightSet>::op_kleene(node<WeightSet>* e)
      -> node_t*
    {
      if (node<WeightSet>::ZERO == e->type())
        {
          // Trivial identity
          // (0)* == 1
          delete e;
          return new one<WeightSet>;
        }
      else
        return new kleene<WeightSet>(e);
    }

    template <class WeightSet>
    inline
    one<WeightSet> *
    factory<WeightSet>::op_one()
    {
      return new one<WeightSet>();
    }

    template <class WeightSet>
    inline
    zero<WeightSet>*
    factory<WeightSet>::op_zero()
    {
      return new zero<WeightSet>();
    }

    template <class WeightSet>
    inline
    word<WeightSet>*
    factory<WeightSet>::op_word(std::string* w)
    {
      if (ws_ != nullptr)
        return new word<WeightSet>(w, *ws_);
      else
        return new word<WeightSet>(w);
    }

    template <class WeightSet>
    inline
    typename factory<WeightSet>::weight_str_container*
    factory<WeightSet>::op_weight(std::string* w)
    {
      weight_str_container* res = new weight_str_container();
      res->push_front(w);
      return res;
    }

    template <class WeightSet>
    exp*
    factory<WeightSet>::op_weight(weight_str_container* w, exp* e)
    {
      if (!w)
        return e;
      // if w
      auto expr = down_cast<node_t*>(e);
      switch (expr->weight_type())
        {
        case node_t::L_WEIGHT:
          {
            auto lweight = down_cast<left_weighted<WeightSet>*>(expr);
            return op_weight(lweight, w);
          }

        case node_t::LR_WEIGHT:
          {
            auto lrweight = down_cast<weighted<WeightSet>*>(expr);
            return op_weight(w, lrweight);
          }

        default:
          assert(false);
          return 0;
        }
    }

    template <class WeightSet>
    exp*
    factory<WeightSet>::op_weight(exp* e, weight_str_container* w)
    {
      if (!w)
        return e;
      // if w
      auto expr = down_cast<node_t*>(e);
      switch (expr->weight_type())
        {
        case node_t::L_WEIGHT:
          {
            auto rweight = down_cast<left_weighted<WeightSet>*>(expr);
            return op_weight(rweight, w);
          }
        case node_t::LR_WEIGHT:
          {
            auto rweight = down_cast<weighted<WeightSet>*>(expr);
            return op_weight(rweight, w);
          }
        default:
          assert(false);
          return 0;
        }
    }

    template<class WeightSet>
    auto
    factory<WeightSet>::op_weight(left_weighted<WeightSet>* e,
                                  weight_str_container* w)
      -> node_t*
    {
      for (auto i : *w)
        {
          weight_t new_weight = ws_->op_conv(*i);
          if (ws_->is_zero(new_weight))
            {
              delete e;
              return new zero<WeightSet>();
            }
          e->left_weight() = ws_->mul(e->left_weight(), new_weight);
        }
      return e;
    }

    template<class WeightSet>
    auto
    factory<WeightSet>::op_weight(weight_str_container* w,
                                  weighted<WeightSet>* e)
      -> node_t*
    {
      for (auto i : *w)
        {
          weight_t new_weight = ws_->op_conv(*i);
          if (ws_->is_zero(new_weight))
            {
              delete e;
              return new zero<WeightSet>();
            }
          e->left_weight() = ws_->mul(e->left_weight(), new_weight);
        }
      return e;
    }

    template<class WeightSet>
    auto
    factory<WeightSet>::op_weight(weighted<WeightSet>* e,
                                  weight_str_container* w)
      -> node_t*
    {
      for (auto i : *w)
        {
          weight_t new_weight = ws_->op_conv(*i);
          if (ws_->is_zero(new_weight))
            {
              delete e;
              return new zero<WeightSet>();
            }
          e->right_weight() = ws_->mul(e->right_weight(), new_weight);
        }
      return e;
    }

    template <class WeightSet>
    inline
    typename factory<WeightSet>::weight_str_container*
    factory<WeightSet>::op_weight(weight_str* w, weight_str_container* l)
    {
      l->push_front(w);
      return l;
    }

    template <class WeightSet>
    inline
    typename factory<WeightSet>::weight_str_container*
    factory<WeightSet>::op_weight(weight_str_container* l, weight_str* w)
    {
      l->push_front(w);
      return l;
    }

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_RAT_EXP_HXX_
