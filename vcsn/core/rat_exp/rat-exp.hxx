#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HXX_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HXX_

# include <algorithm>
# include <cassert>

# include <misc/cast.hh>
# include <core/rat_exp/rat-exp.hh>
# include <core/rat_exp/node.hh>

namespace vcsn {
  namespace rat_exp {

    template<class WeightSet>
    inline
    concat *
    RatExp<WeightSet>::op_mul(exp *e)
    {
      concat *res = new concat();
      res->push_front(e);
      return res;
    }

    template<class WeightSet>
    inline
    concat *
    RatExp<WeightSet>::op_mul(exp *l,    exp *r)
    {
      concat *res = new concat();
      res->push_front(r);
      res->push_front(l);
      return res;
    }

    template<class WeightSet>
    inline
    concat *
    RatExp<WeightSet>::op_mul(exp *l, concat *r)
    {
      r->push_front(l);
      return l;
    }

    template<class WeightSet>
    inline
    concat *
    RatExp<WeightSet>::op_mul(concat *l, exp *r)
    {
      l->push_back(r);
      return l;
    }

    template<class WeightSet>
    inline
    plus *
    RatExp<WeightSet>::op_add(exp *l,  exp *r)
    {
      plus *res = new plus;
      res->push_front(r);
      res->push_front(l);
      return res;
    }

    template<class WeightSet>
    inline
    plus *
    RatExp<WeightSet>::op_add(exp *l,  plus *r)
    {
      r->push_front(l);
      return r;
    }

    template<class WeightSet>
    inline
    plus *
    RatExp<WeightSet>::op_add(plus *l, exp *r)
    {
      l->push_front(r);
      return l;
    }

    template<class WeightSet>
    inline
    kleene *
    RatExp<WeightSet>::op_kleene(exp *e)
    {
      return new kleene(e);
    }

    template<class WeightSet>
    inline
    one  *
    RatExp<WeightSet>::op_one()
    {
      return new one();
    }

    template<class WeightSet>
    inline
    zero *
    RatExp<WeightSet>::op_zero()
    {
      return new zero();
    }

    inline
    void
    push_front_weights(weights_type *that, weights_type *here)
    {
      for(weight_type *w : *that)
        here->push_front(w);
    }

    inline
    void
    push_back_weights(weights_type *that, weights_type *here)
    {
      for(weight_type *w : *that)
        here->push_back(w);
    }

    template<class WeightSet>
    inline
    left_weight *
    RatExp<WeightSet>::op_weight(weights_type *l, exp *r)
    {
      if(exp::LEFT_WEIGHT == r->getType())
      {
        left_weight *tmp = down_cast<left_weight *> (r);
        assert(tmp);
        push_front_weights(l, tmp->get_weight());
        return tmp;
      }
      else
      {
      // l->remove_if(weight_set_.is_one);
      // FIXME: check trivial identity
      // if(l.end() != find_if(l.begin(), l.end(), weight_set.is_zero))
        return new left_weight(l, r);
      // else
      // {
      //   delete l;
      //   delete r;
      //   return nullptr;
      // }
      }
    }

    template<class WeightSet>
    inline
    right_weight *
    RatExp<WeightSet>::op_weight(exp *l, weights_type *r)
    {
      if(exp::RIGHT_WEIGHT == l->getType())
      {
        right_weight *tmp = down_cast<right_weight *>(l);
        assert(tmp);
        push_back_weights(r, tmp->get_weight());
        return tmp;
      }
      else
      {
      // r->remove_if(weight_set_.is_one);
      // FIXME: check trivial identity
      // if(r.end() != find_if(r.begin(), r.end(), weight_set.is_zero))
        return new right_weight(l, r);
      // else
      // {
      //   delete l;
      //   delete r;
      //   return nullptr;
      // }
      }
    }

    template<class WeightSet>
    inline
    word *
    RatExp<WeightSet>::op_word(std::string *w)
    {
      return new word(w);
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExp<WeightSet>::op_weight(weight_type *w)
    {
      weights_type *res = new weights_type();
      res->push_front(w);
      return res;
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExp<WeightSet>::op_weight(weight_type *w, weights_type *l)
    {
      l->push_front(w);
      return l;
    }

    template<class WeightSet>
    inline
    weights_type *
    RatExp<WeightSet>::op_weight(weights_type *l, weight_type *w)
    {
      l->push_front(w);
      return l;
    }

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HXX_
