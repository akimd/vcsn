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
    -> zero_t*
  {
    return new zero_t(ws_.unit());
  }

  DEFINE::unit() const
    -> one_t*
  {
    return new one_t(ws_.unit());
  }

  DEFINE::atom(const std::string& w) const
    -> atom_t*
  {
    for (auto c: w)
      if (!gs_.has(c))
        throw std::domain_error("invalid word: " + w + ": invalid letter: " + c);
    return new atom_t(ws_.unit(), w);
  }

  DEFINE::add(exp_t* l, exp_t* r) const
    -> exp_t*
  {
    auto left = down_cast<node_t*>(l);
    auto right = down_cast<node_t*>(r);
    return add(left, right);
  }

  DEFINE::mul(exp_t* l, exp_t* r) const
    -> exp_t*
  {
    auto left = down_cast<node_t*>(l);
    auto right = down_cast<node_t*>(r);
    return mul(left, right);
  }

  DEFINE::star(exp_t* e) const
    -> exp_t*
  {
    return star(down_cast<node_t*>(e));
  }


  DEFINE::weight(std::string* w, exp_t* e) const
    -> exp_t*
  {
    // The weight might not be needed (e = 0), but check its syntax
    // anyway.
    auto res = weight(ws_.conv(*w), down_cast<node_t*>(e));
    delete w;
    return res;
  }

  DEFINE::weight(exp_t* e, std::string* w) const
    -> exp_t*
  {
    auto res = weight(down_cast<node_t*>(e), ws_.conv(*w));
    delete w;
    return res;
  }



  /*-----------------.
  | Concrete types.  |
  `-----------------*/

  DEFINE::add(node_t* l, node_t* r) const
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
        sum_t* res = new sum_t(ws_.unit(), ws_.unit());
        res->push_front(r);
        res->push_front(l);
        return res;
      }
  }


  DEFINE::mul(node_t* l, node_t* r) const
    -> node_t*
  {
    node_t* res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0
    if (node_t::ZERO == l->type())
      {
        delete r;
        res = l;
      }
    else if (node_t::ZERO == r->type())
      {
        delete l;
        res = r;
      }
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (node_t::ONE == r->type())
      {
        res = weight(l, r->left_weight());
        delete r;
      }
    else if (node_t::ONE == l->type())
      {
        res = weight(l->left_weight(), r);
        delete l;
      }
    // END: Trivial Identity
    else if (auto left = maybe_down_cast<prod_t*>(l))
      {
        if (auto right = maybe_down_cast<prod_t*>(r))
          {
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
    else if (auto right = maybe_down_cast<prod_t*>(r))
      {
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

  DEFINE::star(node_t* e) const
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
      return new star_t(ws_.unit(), ws_.unit(), e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, node_t* e) const
    -> node_t*
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

  DEFINE::weight(node_t* e, const weight_t& w) const
    -> node_t*
  {
    // Trivial identity $T_K$: {k}0 => 0, 0{k} => 0.
    if (e->type() != node_t::ZERO)
      {
        if (ws_.is_zero(w))
          {
            delete e;
            e = zero();
          }
        else if (auto in = maybe_down_cast<inner_t*>(e))
          in->right_weight() = ws_.mul(in->right_weight(), w);
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
    const auto* down = down_cast<const node_t*>(v);
    rat::printer<weightset_t> print(o, ws_);
    down->accept(print);
    return o;
  }

#undef DEFINE

} // namespace vcsn
