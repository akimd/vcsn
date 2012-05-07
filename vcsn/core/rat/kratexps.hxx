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
    -> value_t
  {
    return std::make_shared<zero_t>(ws_.unit());
  }

  DEFINE::unit() const
    -> value_t
  {
    return std::make_shared<one_t>(ws_.unit());
  }

  DEFINE::atom(const std::string& w) const
    -> value_t
  {
    for (auto c: w)
      if (!gs_.has(c))
        throw std::domain_error("invalid word: " + w
                                + ": invalid letter: " + c);
    return std::make_shared<atom_t>(ws_.unit(), w);
  }

  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_pointer_cast<const node_t>(l);
    auto right = down_pointer_cast<const node_t>(r);
    return add(left, right);
  }

  DEFINE::mul(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_pointer_cast<const node_t>(l);
    auto right = down_pointer_cast<const node_t>(r);
    return mul(left, right);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    return star(down_pointer_cast<const node_t>(e));
  }


  DEFINE::weight(std::string* w, value_t e) const
    -> value_t
  {
    // The weight might not be needed (e = 0), but check its syntax
    // anyway.
    auto v = ws_.conv(*w);
    delete w;
    // Trivial identity $T_K$: {k}0 => 0, {0}x => 0.
    if (e->type() == node_t::ZERO || ws_.is_zero(v))
      return zero();
    else
      return weight(v, down_pointer_cast<const node_t>(e));
  }

  DEFINE::weight(value_t e, std::string* w) const
    -> value_t
  {
    auto v = ws_.conv(*w);
    delete w;
    // Trivial identity $T_K$: 0{k} => 0, x{0} => 0.
    if (e->type() == node_t::ZERO || ws_.is_zero(v))
      return zero();
    else
      return weight(down_pointer_cast<const node_t>(e), v);
  }



  /*-----------------.
  | Concrete types.  |
  `-----------------*/

  DEFINE::gather(nodes_t& res, rat::exp::type_t type, kvalue_t v) const
    -> void
  {
    assert(type == node_t::SUM || type == node_t::PROD);
    if (v->type() == type)
      for (auto n: *down_pointer_cast<const nary_t>(v))
        res.push_back(n);
    else
      res.push_back(v);
  }

  DEFINE::gather(rat::exp::type_t type, kvalue_t l, kvalue_t r) const
    -> nodes_t
  {
    assert(type == node_t::SUM || type == node_t::PROD);
    nodes_t res;
    gather(res, type, l);
    gather(res, type, r);
    return res;
  }

  DEFINE::add(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    // Trivial Identity
    // E+0 = 0+E = E
    if (l->type() == node_t::ZERO)
      return r;
    else if (r->type() == node_t::ZERO)
      return l;
    // END: Trivial Identity
    else
      return std::make_shared<sum_t>(ws_.unit(), ws_.unit(),
                                     gather(node_t::SUM, l, r));
  }


  DEFINE::mul(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    kvalue_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0.
    if (l->type() == node_t::ZERO)
      res = l;
    else if (r->type() == node_t::ZERO)
      res = r;
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (r->type() == node_t::ONE)
      res = weight(l, r->left_weight());
    else if (l->type() == node_t::ONE)
      res = weight(l->left_weight(), r);
    // END: Trivial Identity
    else
      res = std::make_shared<prod_t>(ws_.unit(), ws_.unit(),
                                     gather(node_t::PROD, l, r));
    return res;
  }

  DEFINE::star(kvalue_t e) const
    -> value_t
  {
    if (e->type() == node_t::ZERO)
      // Trivial identity
      // (0)* == 1
      return unit();
//    else if (e->type() == node_t::ONE)
//      // Trivial identity
//      // (k1)* == (k*)1
//      return weight(ws_.star(e->left_weight()),
//                    down_pointer_cast<const leaf_t>(unit()));
    else
      return std::make_shared<star_t>(ws_.unit(), ws_.unit(), e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, kvalue_t e) const
    -> kvalue_t
  {
    auto res = std::const_pointer_cast<node_t>(e->clone());
    res->left_weight() = ws_.mul(w, e->left_weight());
    return res;
  }

  DEFINE::weight(kvalue_t e, const weight_t& w) const
    -> kvalue_t
  {
    using wvalue_t = typename node_t::wvalue_t;
    if (e->is_inner())
      {
        auto inner = down_pointer_cast<const inner_t>(e);
        auto res = std::const_pointer_cast<inner_t>(inner->clone());
        res->right_weight() = ws_.mul(inner->right_weight(), w);
        return res;
      }
    else
      {
        // Not the same as calling weight(w, e), as the product might
        // not be commutative.
        wvalue_t res = std::const_pointer_cast<node_t>(e->clone());
        res->left_weight() = ws_.mul(e->left_weight(), w);
        return res;
      }
  }

  /*---------------------------------.
  | kratexps as a WeightSet itself.  |
  `---------------------------------*/

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return v->type() == node_t::ZERO;
  }

  DEFINE::is_unit(value_t v) const
    -> bool
  {
    return v->type() == node_t::ONE;
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
    rat::printer<weightset_t> print(o, ws_);
    print(down_pointer_cast<const node_t>(v));
    return o;
  }

#undef DEFINE

} // namespace vcsn
