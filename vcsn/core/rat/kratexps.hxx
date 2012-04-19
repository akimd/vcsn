#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/misc/cast.hh>
#include <vcsn/core/rat/node.hh>
#include <vcsn/core/rat/printer.hh>
#include <vcsn/io/parse-rat-exp.hh>

namespace vcsn
{

  /*-----------.
  | kratexps.  |
  `-----------*/

  template <typename GenSet, typename WeightSet>
  kratexps<GenSet, WeightSet>::kratexps(const GenSet& gs, const WeightSet& ws)
    : super_type()
    , gs_(gs)
    , ws_(ws)
  {}

  template <typename GenSet, typename WeightSet>
  template <typename T>
  kratexps<GenSet, WeightSet>::kratexps(const GenSet& gs, const T& t)
    : kratexps(gs, dynamic_cast<const weightset_t&>(t))
  {}


  /*-----------------------------------------------------------.
  | Implementation of abstract_kratexps pure virtual methods.  |
  `-----------------------------------------------------------*/

#define DEFINE                                          \
  template <typename GenSet, typename WeightSet>        \
  inline                                                \
  auto                                                  \
  kratexps<GenSet, WeightSet>

  DEFINE::zero() const
    -> kvalue_t
  {
    return new zero_t(ws_.unit());
  }

  DEFINE::unit() const
    -> kvalue_t
  {
    return new one_t(ws_.unit());
  }

  DEFINE::atom(const std::string& w) const
    -> kvalue_t
  {
    for (auto c: w)
      if (!gs_.has(c))
        throw std::domain_error("invalid word: " + w + ": invalid letter: " + c);
    return new atom_t(ws_.unit(), w);
  }

  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_cast<kvalue_t>(l);
    auto right = down_cast<kvalue_t>(r);
    return add(left, right);
  }

  DEFINE::mul(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_cast<kvalue_t>(l);
    auto right = down_cast<kvalue_t>(r);
    return mul(left, right);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    return star(down_cast<kvalue_t>(e));
  }


  DEFINE::weight(std::string* w, value_t e) const
    -> value_t
  {
    // The weight might not be needed (e = 0), but check its syntax
    // anyway.
    auto res = weight(ws_.conv(*w), down_cast<kvalue_t>(e));
    delete w;
    return res;
  }

  DEFINE::weight(value_t e, std::string* w) const
    -> value_t
  {
    auto res = weight(down_cast<kvalue_t>(e), ws_.conv(*w));
    delete w;
    return res;
  }



  /*-----------------.
  | Concrete types.  |
  `-----------------*/

  DEFINE::add(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    // Trivial Identity
    // E+0 = 0+E = E
    if (l->type() == node_t::ZERO)
      {
        delete l;
        return r;
      }
    if (r->type() == node_t::ZERO)
      {
        delete r;
        return l;
      }
    // END: Trivial Identity

    if (l->type() == node_t::SUM)
      {
        auto res = down_cast<sum_t*>(l);
        if (r->type() == node_t::SUM)
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
    else if (r->type() == node_t::SUM)
      {
        auto res = down_cast<sum_t*>(r);
        res->push_front(l);
        return res;
      }
    else
      {
        sum_t* res = new sum_t(ws_.unit(), ws_.unit());
        res->push_back(l);
        res->push_back(r);
        return res;
      }
  }


  DEFINE::mul(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    kvalue_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0
    if (l->type() == node_t::ZERO)
      {
        delete r;
        res = l;
      }
    else if (r->type() == node_t::ZERO)
      {
        delete l;
        res = r;
      }
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (r->type() == node_t::ONE)
      {
        res = weight(l, r->left_weight());
        delete r;
      }
    else if (l->type() == node_t::ONE)
      {
        res = weight(l->left_weight(), r);
        delete l;
      }
    // END: Trivial Identity
    else if (l->type() == node_t::PROD)
      {
        auto left = down_cast<prod_t*>(l);
        if (r->type() == node_t::PROD)
          {
            auto right = down_cast<prod_t*>(r);
            left->splice(left->end(), *right);
            delete right;
            res = left;
          }
        else
          {
            left->push_back(r);
            res = left;
          }
      }
    else if (r->type() == node_t::PROD)
      {
        auto right = down_cast<prod_t*>(r);
        right->push_front(l);
        res = right;
      }
    else
      {
        auto prod = new prod_t(ws_.unit(), ws_.unit());
        prod->push_back(l);
        prod->push_back(r);
        res = prod;
      }
    return res;
  }

  DEFINE::star(kvalue_t e) const
    -> kvalue_t
  {
    if (e->type() == node_t::ZERO)
      {
        // Trivial identity
        // (0)* == 1
        delete e;
        return unit();
      }
    else
      return new star_t(ws_.unit(), ws_.unit(), e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, kvalue_t e) const
    -> kvalue_t
  {
    // Trivial identity $T_K$: {k}0 => 0, 0{k} => 0.
    if (e->type() != node_t::ZERO)
      {
        if (ws_.is_zero(w))
          {
            delete e;
            e = zero();
          }
        else
          e->left_weight() = ws_.mul(w, e->left_weight());
      }
    return e;
  }

  DEFINE::weight(kvalue_t e, const weight_t& w) const
    -> kvalue_t
  {
    // Trivial identity $T_K$: {k}0 => 0, 0{k} => 0.
    if (e->type() != node_t::ZERO)
      {
        if (ws_.is_zero(w))
          {
            delete e;
            e = zero();
          }
        else if (e->is_inner())
          {
            auto in = down_cast<inner_t*>(e);
            in->right_weight() = ws_.mul(in->right_weight(), w);
          }
        else
          {
            auto leaf = down_cast<leaf_t*>(e);
            leaf->left_weight() = ws_.mul(leaf->left_weight(), w);
          }
      }
    return e;
  }

  /*---------------------------------.
  | kratexps as a WeightSet itself.  |
  `---------------------------------*/

  DEFINE::is_unit(value_t v) const
    -> bool
  {
    return dynamic_cast<one_t*>(v);
  }

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return dynamic_cast<zero_t*>(v);
  }

  DEFINE::conv(const std::string& s) const
    -> value_t
  {
    vcsn::rat::driver d(*this);
    if (value_t res = d.parse_string(s))
      return res;
    throw std::domain_error(d.errors);
  }

  DEFINE::print(std::ostream& o, const value_t v) const
    -> std::ostream&
  {
    const auto* down = down_cast<const kvalue_t>(v);
    rat::printer<weightset_t> print(o, ws_);
    down->accept(print);
    return o;
  }

#undef DEFINE

} // namespace vcsn
