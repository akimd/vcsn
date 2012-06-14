#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/core/rat/kratexp.hh>
#include <vcsn/io/driver.hh>
#include <vcsn/io/parse-rat-exp.hh>
#include <vcsn/misc/cast.hh>

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

  DEFINE::atom(const word_t& w) const
    -> kvalue_t
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


  DEFINE::concat(kvalue_t l, kvalue_t r) const
    -> kvalue_t
  {
    return concat(l, r, kind_t());
  }

  DEFINE::zero() const
    -> kvalue_t
  {
    return zero(weightset()->unit());
  }

  DEFINE::unit() const
    -> kvalue_t
  {
    return unit(weightset()->unit());
  }

  DEFINE::zero(const weight_t& w) const
    -> kvalue_t
  {
    return std::make_shared<zero_t>(w);
  }

  DEFINE::unit(const weight_t& w) const
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

  DEFINE::concat(kvalue_t l, kvalue_t r, labels_are_words) const
    -> kvalue_t
  {
    if (r->type() == kratexp_t::ATOM)
      {
        if (weightset()->is_unit(r->left_weight()))
          switch (l->type())
            {
            case kratexp_t::ATOM:
              if (weightset()->is_unit(l->left_weight()))
                return atom(genset()->concat(down_pointer_cast<const atom_t>(l)->value(),
                                             down_pointer_cast<const atom_t>(r)->value()));
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

  DEFINE::concat(kvalue_t l, kvalue_t r, labels_are_letters) const
    -> kvalue_t
  {
    return mul(l, r);
  }

  DEFINE::star(kvalue_t e) const
    -> kvalue_t
  {
    if (e->type() == kratexp_t::ZERO)
      // Trivial identity
      // (0)* == 1
      return unit();
    else
      return std::make_shared<star_t>(weightset()->unit(), 
                                      weightset()->unit(),
                                      e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, kvalue_t e) const
    -> kvalue_t
  {
    // Trivial identity $T_K$: {k}0 => 0, {0}x => 0.
    if (e->type() == kratexp_t::ZERO || weightset()->is_zero(w))
      return zero();
    else
      {
        auto res = std::const_pointer_cast<kratexp_t>(e->clone());
        res->left_weight() = weightset()->mul(w, e->left_weight());
        return res;
      }
  }

  DEFINE::weight(kvalue_t e, const weight_t& w) const
    -> kvalue_t
  {
    // Trivial identity $T_K$: 0{k} => 0, x{0} => 0.
    if (e->type() == kratexp_t::ZERO || weightset()->is_zero(w))
      return zero();
    else if (e->is_inner())
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
        using wvalue_t = typename kratexp_t::wvalue_t;
        wvalue_t res = std::const_pointer_cast<kratexp_t>(e->clone());
        res->left_weight() = weightset()->mul(e->left_weight(), w);
        return res;
      }
  }

  /*-----------------------------------.
  | kratexpset as a WeightSet itself.  |
  `-----------------------------------*/

  DEFINE::is_zero(kvalue_t v) const
    -> bool
  {
    return v->type() == kratexp_t::ZERO;
  }

  DEFINE::is_unit(kvalue_t v) const
    -> bool
  {
    return v->type() == kratexp_t::ONE && weightset()->is_unit(v->left_weight());
  }

  DEFINE::conv(const std::string& s) const
    -> kvalue_t
  {
    vcsn::concrete_abstract_kratexpset<context_t> fac{context()};
    vcsn::rat::driver d(fac);
    if (auto res = d.parse_string(s))
      return down_pointer_cast<const kratexp_t>(res);
    throw std::domain_error(d.errors);
  }

  DEFINE::print(std::ostream& o, const kvalue_t v) const
    -> std::ostream&
  {
    printer_t print{o, context()};
    print(down_pointer_cast<const kratexp_t>(v));
    return o;
  }

#undef DEFINE

} // namespace vcsn
