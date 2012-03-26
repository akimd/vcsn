#include <algorithm>
#include <cassert>

#include <vcsn/misc/cast.hh>
#include <vcsn/core/rat/node.hh>
#include <vcsn/core/rat/printer.hh>
#include <vcsn/io/parse-rat-exp.hh>

namespace vcsn
{


  /*-----------.
  | factory_.  |
  `-----------*/

  template <class WeightSet>
  factory_<WeightSet>::factory_(const WeightSet& ws)
    : super_type()
    , ws_(&ws)
  {}

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::mul(exp_t* l, exp_t* r) const
    -> exp_t*
  {
    auto left = down_cast<node_t*>(l);
    auto right = down_cast<node_t*>(r);
    return mul(left, right);
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::add(exp_t* l, exp_t* r) const
    -> exp_t*
  {
    auto left = down_cast<node_t*>(l);
    auto right = down_cast<node_t*>(r);
    return add(left, right);
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::star(exp_t* e) const
    -> exp_t*
  {
    return star(down_cast<node_t*>(e));
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::mul(node_t* l, node_t* r) const
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

    if (node_t::PROD == l->type())
      {
        auto left = down_cast<prod_t*>(l);
        if (node_t::PROD == r->type())
          {
            auto right = down_cast<prod_t*>(r);
            left->splice(left->end(), *right);
            delete right;
            return left;
          }
        else
          {
            left->push_back(r);
            return left;
          }
      }
    else if (node_t::PROD == r->type())
      {
        auto right = down_cast<prod_t*>(r);
        right->push_front(l);
        return right;
      }
    else
      {
        auto res = new prod_t(ws_->unit(), ws_->unit());
        res->push_back(l);
        res->push_back(r);
        return res;
      }
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::add(node_t* l, node_t* r) const
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

    if (node_t::SUM == l->type())
      {
        auto res = down_cast<sum_t*>(l);
        if (node_t::SUM == r->type())
          {
            auto right = down_cast<sum_t*>(r);
            res->splice(res->end(), *right);
            delete right;
          }
        else
          {
            res->push_back(r);
          }
        return res;
      }
    else if (node_t::SUM == r->type())
      {
        auto res = down_cast<sum_t*>(r);
        res->push_front(l);
        return res;
      }
    else
      {
        sum_t* res = new sum_t(ws_->unit(), ws_->unit());
        res->push_front(r);
        res->push_front(l);
        return res;
      }
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::star(node_t* e) const
    -> node_t*
  {
    if (node_t::ZERO == e->type())
      {
        // Trivial identity
        // (0)* == 1
        delete e;
        return unit();
      }
    else
      return new star_t(ws_->unit(), ws_->unit(), e);
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::unit() const
    -> one_t*
  {
    return new one_t(ws_->unit());
  }

  template <class WeightSet>
  inline
  bool
  factory_<WeightSet>::is_unit(value_t v) const
  {
    return dynamic_cast<one_t*>(v);
  }

  template <class WeightSet>
  inline
  bool
  factory_<WeightSet>::show_unit() const
  {
    return false;
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::zero() const
    -> zero_t*
  {
    return new zero_t(ws_->unit());
  }

  template <class WeightSet>
  inline
  bool
  factory_<WeightSet>::is_zero(value_t v) const
  {
    return dynamic_cast<zero_t*>(v);
  }

  template <class WeightSet>
  inline
  auto
  factory_<WeightSet>::atom(std::string* w) const
    -> atom_t*
  {
    return new atom_t(ws_->unit(), w);
  }


  template <class WeightSet>
  auto
  factory_<WeightSet>::weight(weight_str_container* w, exp_t* e) const
    -> exp_t*
  {
    if (w)
      return weight(down_cast<node_t*>(e), w);
    else
      return e;
  }

  template <class WeightSet>
  auto
  factory_<WeightSet>::weight(exp_t* e, weight_str_container* w) const
    -> exp_t*
  {
    if (!w)
      return e;
    // if w
    auto expr = down_cast<node_t*>(e);
    if (expr->is_leaf())
      {
        auto rweight = down_cast<leaf_t*>(expr);
        return weight(rweight, w);
      }
    else
      {
        auto rweight = down_cast<inner_t*>(expr);
        return weight(rweight, w);
      }
  }

  template<class WeightSet>
  auto
  factory_<WeightSet>::weight(leaf_t* e,
                              weight_str_container* w) const
    -> node_t*
  {
    for (auto i : *w)
      {
        weight_t new_weight = ws_->conv(*i);
        if (ws_->is_zero(new_weight))
          {
            delete e;
            return zero();
          }
        e->left_weight() = ws_->mul(e->left_weight(), new_weight);
      }
    return e;
  }

  template<class WeightSet>
  auto
  factory_<WeightSet>::weight(weight_str_container* w,
                              inner_t* e) const
    -> node_t*
  {
    for (auto i : *w)
      {
        weight_t new_weight = ws_->conv(*i);
        if (ws_->is_zero(new_weight))
          {
            delete e;
            return zero();
          }
        e->left_weight() = ws_->mul(e->left_weight(), new_weight);
      }
    return e;
  }

  template<class WeightSet>
  auto
  factory_<WeightSet>::weight(inner_t* e,
                              weight_str_container* w) const
    -> node_t*
  {
    for (auto i : *w)
      {
        weight_t new_weight = ws_->conv(*i);
        if (ws_->is_zero(new_weight))
          {
            delete e;
            return zero();
          }
        e->right_weight() = ws_->mul(e->right_weight(), new_weight);
      }
    return e;
  }

  template<class WeightSet>
  auto
  factory_<WeightSet>::conv(const std::string& s) const
    -> value_t
  {
    vcsn::rat::driver d(*this);
    return d.parse_string(s);
  }

  template<class WeightSet>
  std::ostream&
  factory_<WeightSet>::print(std::ostream& o, const value_t v) const
  {
    const auto* down = down_cast<const node_t*>(v);
    rat::printer<weightset_t> print(o, *ws_);
    down->accept(print);
    return o;
  }

} // namespace vcsn
