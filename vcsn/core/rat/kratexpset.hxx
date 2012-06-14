#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/misc/cast.hh>
#include <vcsn/core/rat/kratexp.hh>
#include <vcsn/io/parse-rat-exp.hh>

namespace vcsn
{

  /*-------------------------------------------------------------.
  | Implementation of abstract_kratexpset pure virtual methods.  |
  `-------------------------------------------------------------*/

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  kratexpset<Context>

  DEFINE::zero() const
    -> value_t
  {
    return zero_();
  }

  DEFINE::unit() const
    -> value_t
  {
    return unit_();
  }

  DEFINE::atom(const word_t& w) const
    -> value_t
  {
    return atom_<kind_t>(w);
  }

  template <typename Context>
  template <typename K>
  inline
  auto
  kratexpset<Context>::atom_(const word_t& w) const
    -> typename std::enable_if<std::is_same<K, labels_are_letters>::value,
                               kvalue_t>::type
  {
    if (w.size() != 1)
      throw std::domain_error("invalid atom: " + w);
    letter_t l = w[0];
    if (!genset()->has(l))
      throw std::domain_error("invalid word: " + w
                              + ": invalid letter: " + l);
    return std::make_shared<atom_t>(weightset()->unit(), l);
  }

  template <typename Context>
  template <typename K>
  inline
  auto
  kratexpset<Context>::atom_(const word_t& w) const
    -> typename std::enable_if<std::is_same<K, labels_are_words>::value,
                               kvalue_t>::type
  {
    for (auto l: w)
      if (!genset()->has(l))
          throw std::domain_error("invalid word: " + w
                                  + ": invalid letter: " + l);
    return std::make_shared<atom_t>(weightset()->unit(), w);
  }


  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_pointer_cast<const kratexp_t>(l);
    auto right = down_pointer_cast<const kratexp_t>(r);
    return add(left, right);
  }

  DEFINE::mul(value_t l, value_t r) const
    -> value_t
  {
    auto left = down_pointer_cast<const kratexp_t>(l);
    auto right = down_pointer_cast<const kratexp_t>(r);
    return mul(left, right);
  }

  DEFINE::concat(value_t l, value_t r) const
    -> value_t
  {
    return concat(l, r, kind_t());
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    if (e->type() == kratexp_t::ZERO)
      // Trivial identity
      // (0)* == 1
      return unit();
    else
      return star(down_pointer_cast<const kratexp_t>(e));
  }


  DEFINE::weight(std::string* w, value_t e) const
    -> value_t
  {
    // The weight might not be needed (e = 0), but check its syntax
    // anyway.
    auto v = weightset()->conv(*w);
    delete w;
    // Trivial identity $T_K$: {k}0 => 0, {0}x => 0.
    if (e->type() == kratexp_t::ZERO || weightset()->is_zero(v))
      return zero();
    else
      return weight(v, down_pointer_cast<const kratexp_t>(e));
  }

  DEFINE::weight(value_t e, std::string* w) const
    -> value_t
  {
    auto v = weightset()->conv(*w);
    delete w;
    // Trivial identity $T_K$: 0{k} => 0, x{0} => 0.
    if (e->type() == kratexp_t::ZERO || weightset()->is_zero(v))
      return zero();
    else
      return weight(down_pointer_cast<const kratexp_t>(e), v);
  }



  /*-----------------.
  | Concrete types.  |
  `-----------------*/

  DEFINE::zero_() const
    -> kvalue_t
  {
    return zero_(weightset()->unit());
  }

  DEFINE::unit_() const
    -> kvalue_t
  {
    return unit_(weightset()->unit());
  }

  DEFINE::zero_(const weight_t& w) const
    -> kvalue_t
  {
    return std::make_shared<zero_t>(w);
  }

  DEFINE::unit_(const weight_t& w) const
    -> kvalue_t
  {
    return std::make_shared<one_t>(w);
  }


  DEFINE::gather(kratexps_t& res, rat::exp::type_t type, kvalue_t v) const
    -> void
  {
    assert(type == kratexp_t::SUM || type == kratexp_t::PROD);
    if (v->type() == type)
      {
        const auto& nary = *down_pointer_cast<const nary_t>(v);
        if (weightset()->is_unit(nary.left_weight())
            && weightset()->is_unit(nary.right_weight()))
          res.insert(std::end(res), std::begin(nary), std::end(nary));
        else
          res.push_back(v);
      }
    else
      res.push_back(v);
  }

  DEFINE::gather(rat::exp::type_t type, kvalue_t l, kvalue_t r) const
    -> kratexps_t
  {
    assert(type == kratexp_t::SUM || type == kratexp_t::PROD);
    kratexps_t res;
    gather(res, type, l);
    gather(res, type, r);
    return res;
  }

  DEFINE::add(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    // Trivial Identity
    // E+0 = 0+E = E
    kvalue_t res = nullptr;
    if (l->type() == kratexp_t::ZERO)
      res = r;
    else if (r->type() == kratexp_t::ZERO)
      res = l;
    // END: Trivial Identity
    else
      res = std::make_shared<sum_t>(weightset()->unit(),
                                    weightset()->unit(),
                                    gather(kratexp_t::SUM, l, r));
    return res;
  }


  DEFINE::mul(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    kvalue_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0.
    if (l->type() == kratexp_t::ZERO)
      res = l;
    else if (r->type() == kratexp_t::ZERO)
      res = r;
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (r->type() == kratexp_t::ONE)
      res = weight(l, r->left_weight());
    else if (l->type() == kratexp_t::ONE)
      res = weight(l->left_weight(), r);
    // END: Trivial Identity
    else
      res = std::make_shared<prod_t>(weightset()->unit(),
                                     weightset()->unit(),
                                     gather(kratexp_t::PROD, l, r));
    return res;
  }

  DEFINE::concat(value_t l, value_t r, labels_are_words) const
    -> value_t
  {
    if (r->type() == kratexp_t::ATOM)
      {
        auto rhs = down_pointer_cast<const atom_t>(r);
        if (weightset()->is_unit(rhs->left_weight()))
          switch (l->type())
            {
            case kratexp_t::ATOM:
              {
                auto lhs = down_pointer_cast<const atom_t>(l);
                if (weightset()->is_unit(lhs->left_weight()))
                  return atom(genset()->concat(lhs->value(), rhs->value()));
              }
              break;

              // If we are calling word on "(ab).a, b", then we really
              // want "(ab).(ab)".
            case kratexp_t::PROD:
              {
                const auto& prod = *down_pointer_cast<const prod_t>(l);
                kratexps_t kratexps {prod.begin(), prod.end()-1};
                kratexps.push_back
                  (down_pointer_cast<const kratexp_t>(concat(*(prod.end()-1), r)));
                return std::make_shared<prod_t>(weightset()->unit(),
                                                weightset()->unit(),
                                                kratexps);
              }
            default:
              // Fall thru.
              ;
            }
      }
    return mul(l, r);
  }

  DEFINE::concat(value_t l, value_t r, labels_are_letters) const
    -> value_t
  {
    return mul(l, r);
  }

  DEFINE::star(kvalue_t e) const
    -> kvalue_t
  {
    return std::make_shared<star_t>(weightset()->unit(), weightset()->unit(), e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, kvalue_t e) const
    -> kvalue_t
  {
    auto res = std::const_pointer_cast<kratexp_t>(e->clone());
    res->left_weight() = weightset()->mul(w, e->left_weight());
    return res;
  }

  DEFINE::weight(kvalue_t e, const weight_t& w) const
    -> kvalue_t
  {
    using wvalue_t = typename kratexp_t::wvalue_t;
    if (e->is_inner())
      {
        auto inner = down_pointer_cast<const inner_t>(e);
        auto res = std::const_pointer_cast<inner_t>(inner->clone());
        res->right_weight() = weightset()->mul(inner->right_weight(), w);
        return res;
      }
    else
      {
        // Not the same as calling weight(w, e), as the product might
        // not be commutative.
        wvalue_t res = std::const_pointer_cast<kratexp_t>(e->clone());
        res->left_weight() = weightset()->mul(e->left_weight(), w);
        return res;
      }
  }

  /*-----------------------------------.
  | kratexpset as a WeightSet itself.  |
  `-----------------------------------*/

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return v->type() == kratexp_t::ZERO;
  }

  DEFINE::is_unit(value_t v) const
    -> bool
  {
    return is_unit(down_pointer_cast<const kratexp_t>(v));
  }

  DEFINE::is_unit(kvalue_t v) const
    -> bool
  {
    return v->type() == kratexp_t::ONE && weightset()->is_unit(v->left_weight());
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
    printer_t print{o, context()};
    print(down_pointer_cast<const kratexp_t>(v));
    return o;
  }

#undef DEFINE

} // namespace vcsn
