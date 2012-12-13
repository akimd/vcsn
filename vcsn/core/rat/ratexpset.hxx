#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/core/rat/abstract_ratexpset.hh>
#include <vcsn/core/rat/ratexp.hh>
#include <lib/vcsn/rat/driver.hh>// FIXME: non-installed dependency.
#include <vcsn/misc/cast.hh>
#include <vcsn/core/rat/transpose.hh>

namespace vcsn
{

  /*------------------------------------------------------------.
  | Implementation of abstract_ratexpset pure virtual methods.  |
  `------------------------------------------------------------*/

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  ratexpset<Context>


  DEFINE::atom(const label_t& v) const
    -> value_t
  {
    // Bouncing on atom_<context_t> seems useless ---after all we are
    // already parameterized by Context--- but that's needed for
    // enable_if to work.
    return atom_<context_t>(v);
  }

  template <typename Context>
  template <typename Ctx>
  inline
  auto
  ratexpset<Context>::atom_(if_lau<Ctx, label_t> v) const
    -> value_t
  {
    return std::make_shared<atom_t>(weightset()->unit(), v);
  }

  template <typename Context>
  template <typename Ctx>
  inline
  auto
  ratexpset<Context>::atom_(if_lal<Ctx, letter_t> v) const
    -> value_t
  {
    if (!genset()->has(v))
      throw std::domain_error("invalid letter: " + std::string{v});
    return std::make_shared<atom_t>(weightset()->unit(), v);
  }

  template <typename Context>
  template <typename Ctx>
  inline
  auto
  ratexpset<Context>::atom_(const if_law<Ctx, word_t>& w) const
    -> value_t
  {
    for (auto l: w)
      if (!genset()->has(l))
        throw std::domain_error("invalid word: " + w
                                + ": invalid letter: " + std::string{l});
    return std::make_shared<atom_t>(weightset()->unit(), w);
  }


  DEFINE::concat(value_t l, value_t r) const
    -> value_t
  {
    return concat(l, r, kind_t());
  }

  DEFINE::zero() const
    -> value_t
  {
    return zero(weightset()->unit());
  }

  DEFINE::unit() const
    -> value_t
  {
    return unit(weightset()->unit());
  }

  DEFINE::zero(const weight_t& w) const
    -> value_t
  {
    return std::make_shared<zero_t>(w);
  }

  DEFINE::unit(const weight_t& w) const
    -> value_t
  {
    return std::make_shared<one_t>(w);
  }


  DEFINE::gather(ratexps_t& res, rat::exp::type_t type, value_t v) const
    -> void
  {
    assert(type == type_t::sum || type == type_t::prod);
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

  DEFINE::gather(rat::exp::type_t type, value_t l, value_t r) const
    -> ratexps_t
  {
    assert(type == type_t::sum || type == type_t::prod);
    ratexps_t res;
    gather(res, type, l);
    gather(res, type, r);
    return res;
  }

  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    // Trivial Identity
    // E+0 = 0+E = E
    value_t res = nullptr;
    if (l->type() == type_t::zero)
      res = r;
    else if (r->type() == type_t::zero)
      res = l;
    // END: Trivial Identity
    else
      res = std::make_shared<sum_t>(weightset()->unit(),
                                    weightset()->unit(),
                                    gather(type_t::sum, l, r));
    return res;
  }


  DEFINE::mul(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (r->type() == type_t::one)
      res = weight(l, r->left_weight());
    else if (l->type() == type_t::one)
      res = weight(l->left_weight(), r);
    // END: Trivial Identity
    else
      res = std::make_shared<prod_t>(weightset()->unit(),
                                     weightset()->unit(),
                                     gather(type_t::prod, l, r));
    return res;
  }

  DEFINE::concat(value_t l, value_t r, labels_are_unit) const
    -> value_t
  {
    return mul(l, r);
  }

  DEFINE::concat(value_t l, value_t r, labels_are_letters) const
    -> value_t
  {
    return mul(l, r);
  }

  DEFINE::concat(value_t l, value_t r, labels_are_words) const
    -> value_t
  {
    // Implicit concatenation between l and r.
    auto rt = r->type();
    auto lt = l->type();
    if (rt == type_t::atom
        && weightset()->is_unit(r->left_weight()))
      {
        if (lt == type_t::atom
            && weightset()->is_unit(l->left_weight()))
          return atom(genset()->concat
                      (down_pointer_cast<const atom_t>(l)->value(),
                       down_pointer_cast<const atom_t>(r)->value()));
        else if (lt == type_t::prod)
          {
            const auto& prodl = *down_pointer_cast<const prod_t>(l);
            if (weightset()->is_unit(prodl.left_weight())
                && weightset()->is_unit(prodl.right_weight()))
              {
                // Concat of "(ab).a" and "b" is "(ab).(ab)".
                ratexps_t ratexps { prodl.begin(), prodl.end() };
                ratexps.back() = concat(ratexps.back(), r);
                return std::make_shared<prod_t>(weightset()->unit(),
                                                weightset()->unit(),
                                                ratexps);
              }
          }
        }
    // The following cases do not when parsing an expression.
    else if (rt == type_t::prod)
      {
        const auto& prodr = *down_pointer_cast<const prod_t>(r);
        if (weightset()->is_unit(prodr.left_weight())
            && weightset()->is_unit(prodr.right_weight()))
          {
            if (lt == type_t::atom
                && weightset()->is_unit(l->left_weight()))
              {
                // Concat of "a" and "b.(ab)", is "(ab).(ab)".
                ratexps_t ratexps { prodr.begin(), prodr.end() };
                ratexps.front() = concat(l, ratexps.front());
                return std::make_shared<prod_t>(weightset()->unit(),
                                                weightset()->unit(),
                                                ratexps);
              }
            else if (lt == type_t::prod)
              {
                const auto& prodl = *down_pointer_cast<const prod_t>(l);
                if (weightset()->is_unit(prodl.left_weight())
                    && weightset()->is_unit(prodl.right_weight()))
                  {
                    // Concat of "(ab).a" and "b.(ab)" is "(ab).(ab).(ab)".
                    ratexps_t ratexps { prodl.begin(), prodl.end() };
                    ratexps.back() = concat(ratexps.back(), *prodr.begin());
                    ratexps.insert(ratexps.end(),
                                    prodr.begin() + 1, prodr.end());
                    return std::make_shared<prod_t>(weightset()->unit(),
                                                    weightset()->unit(),
                                                    ratexps);
                  }
              }
          }
      }
    // Fall back to explicit concatenation.
    return mul(l, r);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    if (e->type() == type_t::zero)
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

  DEFINE::weight(const weight_t& w, value_t e) const
    -> value_t
  {
    // Trivial identity $T_K$: {k}0 => 0, {0}x => 0.
    if (e->type() == type_t::zero || weightset()->is_zero(w))
      return zero();
    else
      {
        auto res = std::const_pointer_cast<node_t>(e->clone());
        res->left_weight() = weightset()->mul(w, e->left_weight());
        return res;
      }
  }

  DEFINE::weight(value_t e, const weight_t& w) const
    -> value_t
  {
    // Trivial identity $T_K$: 0{k} => 0, x{0} => 0.
    if (e->type() == type_t::zero || weightset()->is_zero(w))
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
        using wvalue_t = typename node_t::wvalue_t;
        wvalue_t res = std::const_pointer_cast<node_t>(e->clone());
        res->left_weight() = weightset()->mul(e->left_weight(), w);
        return res;
      }
  }

  /*----------------------------------.
  | ratexpset as a WeightSet itself.  |
  `----------------------------------*/

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return v->type() == type_t::zero;
  }

  DEFINE::is_unit(value_t v) const
    -> bool
  {
    return (v->type() == type_t::one
            && weightset()->is_unit(v->left_weight()));
  }

  DEFINE::conv(const std::string& s) const
    -> value_t
  {
    vcsn::concrete_abstract_ratexpset<context_t> fac{context()};
    vcsn::rat::driver d(fac);
    if (auto res = d.parse_string(s))
      return down_pointer_cast<const node_t>(res);
    throw std::domain_error(d.errors);
  }

  DEFINE::print(std::ostream& o, const value_t v) const
    -> std::ostream&
  {
    printer_t print{o, context()};
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    details::transposer<Context> tr{context()};
    return tr(v);
  }

#undef DEFINE

} // namespace vcsn
