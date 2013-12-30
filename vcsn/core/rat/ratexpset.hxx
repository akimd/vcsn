#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/core/rat/copy.hh>
#include <vcsn/core/rat/less-than.hh>
#include <vcsn/core/rat/ratexp.hh>
#include <vcsn/core/rat/transpose.hh>
#include <vcsn/dyn/algos.hh> // dyn::read_ratexp_string
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/ratexpset.hh> // dyn::make_ratexpset
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  template <typename Context>
  ratexpset<Context>::ratexpset(const context_t& ctx)
    : ctx_(ctx)
  {}

  template <typename Context>
  std::string
  ratexpset<Context>::sname()
  {
    return "ratexpset<" + context_t::sname() + '>';
  }

  template <typename Context>
  std::string
  ratexpset<Context>::vname(bool full) const
  {
    return "ratexpset<" + context().vname(full) + '>';
  }

  template <typename Context>
  ratexpset<Context>
  ratexpset<Context>::make(std::istream& is)
  {
    // name is, for instance, "ratexpset<lal_char(abcd)_z>".
    eat(is, "ratexpset<");
    auto ctx = Context::make(is);
    eat(is, '>');
    return {ctx};
  }

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  ratexpset<Context>

  DEFINE::context() const -> const context_t&
  {
    return ctx_;
  }

  DEFINE::labelset() const -> const labelset_ptr&
  {
    return ctx_.labelset();
  }

  DEFINE::weightset() const -> const weightset_ptr&
  {
    return ctx_.weightset();
  }

  DEFINE::format(const value_t v) const -> std::string
  {
    std::ostringstream s;
    print(s, v);
    return s.str();
  }


  /*--------------------------------------------------------.
  | Implementation of ratexpset_base pure virtual methods.  |
  `--------------------------------------------------------*/

  DEFINE::atom(const label_t& v) const
    -> value_t
  {
    if (labelset()->is_one(v))
      return one();
    return std::make_shared<atom_t>(v);
  }

  DEFINE::zero() const
    -> value_t
  {
    return std::make_shared<zero_t>();
  }

  DEFINE::one() const
    -> value_t
  {
    return std::make_shared<one_t>();
  }

  template <typename Context>
  template <rat::exp::type_t Type>
  inline
  auto
  ratexpset<Context>::gather(ratexps_t& res, value_t v) const
    -> void
  {
    if (v->type() == Type)
      {
        const auto& nary = *down_pointer_cast<const nary_t<Type>>(v);
        res.insert(std::end(res), std::begin(nary), std::end(nary));
      }
    else
      res.push_back(v);
  }

  template <typename Context>
  template <rat::exp::type_t Type>
  inline
  auto
  ratexpset<Context>::gather(value_t l, value_t r) const
    -> ratexps_t
  {
    ratexps_t res;
    gather<Type>(res, l);
    gather<Type>(res, r);
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
      res = std::make_shared<sum_t>(gather<type_t::sum>(l, r));
    return res;
  }

  DEFINE::type_ignoring_lweight_(value_t e) const
    -> rat::exp::type_t
  {
    if (e->type() == type_t::lweight)
      return down_pointer_cast<const lweight_t>(e)->sub()->type();
    else
      return e->type();
  }

  DEFINE::possibly_implicit_lweight_(value_t e) const
    -> weight_t
  {
    if (e->type() == type_t::lweight)
      return down_pointer_cast<const lweight_t>(e)->weight();
    else
      return weightset()->one();
  }

  DEFINE::unwrap_possible_lweight_(value_t e) const
    -> value_t
  {
    if (e->type() == type_t::lweight)
      return down_pointer_cast<const lweight_t>(e)->sub();
    else
      return e;
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
    // U_K: E.({k}1) ⇒ E{k}, subsuming T: E.1 = E.
    else if (type_ignoring_lweight_(r) == type_t::one)
      res = rmul(l, possibly_implicit_lweight_(r));
    // U_K: ({k}1).E ⇒ {k}E, subsuming T: 1.E = E.
    else if (type_ignoring_lweight_(l) == type_t::one)
      res = lmul(possibly_implicit_lweight_(l), r);
    // END: Trivial Identity
    else
      res = std::make_shared<prod_t>(gather<type_t::prod>(l, r));
    return res;
  }

  DEFINE::intersection(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity.
    // E&0 = 0&E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // <k>1&<h>1 = <k.h>1.
    else if (type_ignoring_lweight_(l) == type_t::one
             && type_ignoring_lweight_(r) == type_t::one)
      res = lmul(weightset()->mul(possibly_implicit_lweight_(l),
                                  possibly_implicit_lweight_(r)),
                 one());
    // <k>a&<h>a = <k.h>a.  <k>a&<h>b = 0.
    else if (type_ignoring_lweight_(l) == type_t::atom
             && type_ignoring_lweight_(r) == type_t::atom)
      {
        auto lhs = down_pointer_cast<const atom_t>(unwrap_possible_lweight_(l))->value();
        auto rhs = down_pointer_cast<const atom_t>(unwrap_possible_lweight_(r))->value();
        if (labelset()->equals(lhs, rhs))
          res = rmul(l, possibly_implicit_lweight_(r));
        else
          res = zero();
      }
    // <k>1&<h>a = 0, <k>a&<h>1 = 0.
    else if (   (type_ignoring_lweight_(l) == type_t::one
                 && type_ignoring_lweight_(r) == type_t::atom)
             || (type_ignoring_lweight_(l) == type_t::atom
                 && type_ignoring_lweight_(r) == type_t::one))
      res = zero();
    // END: Trivial Identity
    else
      res = std::make_shared<intersection_t>(gather<type_t::intersection>(l, r));
    return res;
  }

  DEFINE::shuffle(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity.
    // E:0 = 0:E = E.
    if (l->type() == type_t::zero)
      res = r;
    else if (r->type() == type_t::zero)
      res = l;
    // END: Trivial Identity
    else
      res = std::make_shared<shuffle_t>(gather<type_t::shuffle>(l, r));
    return res;
  }

  DEFINE::concat(value_t l, value_t r) const
    -> value_t
  {
    return concat_(l, r, typename is_law<Context>::type{});
  }

  // Concatenation when not LAW.
  DEFINE::concat_(value_t l, value_t r, std::false_type) const
    -> value_t
  {
    return mul(l, r);
  }

  // Concatenation when LAW.
  DEFINE::concat_(value_t l, value_t r, std::true_type) const
    -> value_t
  {
    // Implicit concatenation between l and r.
    auto rt = r->type();
    auto lt = l->type();
    if (rt == type_t::atom)
      {
        if (lt == type_t::atom)
          return atom(labelset()->concat
                      (down_pointer_cast<const atom_t>(l)->value(),
                       down_pointer_cast<const atom_t>(r)->value()));
        else if (lt == type_t::prod)
          {
            const auto& prodl = *down_pointer_cast<const prod_t>(l);
            // Concat of "(ab).a" and "b" is "(ab).(ab)".
            ratexps_t ratexps { prodl.begin(), prodl.end() };
            ratexps.back() = concat(ratexps.back(), r);
            return std::make_shared<prod_t>(ratexps);
          }
      }
    // The following cases do not occur when parsing an expression.
    else if (rt == type_t::prod)
      {
        const auto& prodr = *down_pointer_cast<const prod_t>(r);
        if (lt == type_t::atom)
          {
            // Concat of "a" and "b.(ab)", is "(ab).(ab)".
            ratexps_t ratexps { prodr.begin(), prodr.end() };
            ratexps.front() = concat(l, ratexps.front());
            return std::make_shared<prod_t>(ratexps);
          }
        else if (lt == type_t::prod)
          {
            const auto& prodl = *down_pointer_cast<const prod_t>(l);
            // Concat of "(ab).a" and "b.(ab)" is "(ab).(ab).(ab)".
            ratexps_t ratexps { prodl.begin(), prodl.end() };
            ratexps.back() = concat(ratexps.back(), *prodr.begin());
            ratexps.insert(ratexps.end(),
                           prodr.begin() + 1, prodr.end());
            return std::make_shared<prod_t>(ratexps);
          }
      }
    // Fall back to explicit concatenation.
    return mul(l, r);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    if (e->type() == type_t::zero)
      // Trivial one
      // (0)* == 1
      return one();
    else
      return std::make_shared<star_t>(e);
  }

  DEFINE::complement(value_t e) const
    -> value_t
  {
    // Trivial identity: (k.E){c} => E{c}, (E.k){c} => E{c}.
    // Without it, derived-term (<2>a)*{c} fails to terminate.
    if (auto w = down_pointer_cast<const lweight_t>(e))
      return complement(w->sub());
    else if (auto w = down_pointer_cast<const rweight_t>(e))
      return complement(w->sub());
    else
      return std::make_shared<complement_t>(e);
  }

  /*----------.
  | weights.  |
  `----------*/

  DEFINE::lmul(const weight_t& w, value_t e) const
    -> value_t
  {
    // Trivial identities $T_K$: <k>0 => 0, <0>E => 0.
    if (e->type() == type_t::zero || weightset()->is_zero(w))
      return zero();
    // Trivial identity: <1>E => E.
    else if (weightset()->is_one(w))
      return e;
    // Trivial identity: <k>(<h>E) => <kh>E.
    else if (e->type() == type_t::lweight)
      {
        const auto &lw = *down_pointer_cast<const lweight_t>(e);
        return lmul(weightset()->mul(w, lw.weight()), lw.sub());
      }
    else
    // General case: <k>E.
      return std::make_shared<lweight_t>(w, e);
  }

  DEFINE::rmul(value_t e, const weight_t& w) const
    -> value_t
  {
    if (! e->is_inner())
    // Leafs only have left weights and lmul takes care of normalization.
      return lmul(w, e);
    // Trivial identities $T_K$: E<0> => 0.
    else if (weightset()->is_zero(w))
      return zero();
    // Trivial identity: E<1> => E.
    else if (weightset()->is_one(w))
      return e;
    // Trivial identity: (E<k>)<h> => E<kh>.
    else if (e->type() == type_t::rweight)
      {
        const auto &rw = *down_pointer_cast<const rweight_t>(e);
        return rmul(rw.sub(), weightset()->mul(rw.weight(), w));
      }
    // Trivial identity: (<k>E)<h> => <k>(E<h>).
    else if (e->type() == type_t::lweight)
      {
        const lweight_t& lw = *down_pointer_cast<const lweight_t>(e);
        return lmul(lw.weight(), rmul(lw.sub(), w));
      }
    // General case: E<k>.
    else
      return std::make_shared<rweight_t>(w, e);
  }

  /*----------------------------------.
  | ratexpset as a WeightSet itself.  |
  `----------------------------------*/

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return v->type() == type_t::zero;
  }

  DEFINE::is_one(value_t v) const
    -> bool
  {
    return (v->type() == type_t::one);
  }

  DEFINE::less_than(value_t lhs, value_t rhs)
    -> bool
  {
    using less_than_t = rat::less_than<ratexpset>;
    less_than_t lt;
    return lt(lhs, rhs);
  }

  DEFINE::conv(self_type, value_t v) const
    -> value_t
  {
    return v;
  }

  DEFINE::conv(b, typename b::value_t v) const
    -> value_t
  {
    return v ? one() : zero();
  }

  DEFINE::conv(const q& ws, typename q::value_t v) const
    -> value_t
  {
    return lmul(weightset()->conv(ws, v), one());
  }

  // Convert from another ratexpset \a ws to ourself.  Requires a full
  // rewrite of the ratexp \a v.
  template <typename Context>
  template <typename Ctx2>
  inline
  auto
  ratexpset<Context>::conv(const ratexpset<Ctx2>& ws,
                           typename ratexpset<Ctx2>::value_t v) const
    -> value_t
  {
    return copy(ws, *this, v);
  }

  DEFINE::conv(std::istream& is) const
    -> value_t
  {
    auto dynres = dyn::read_ratexp(is, dyn::make_ratexpset(*this));
    const auto& res = dynres->template as<ratexpset>();
    return res.ratexp();
  }

  DEFINE::print(std::ostream& o, const value_t v,
		const std::string& format) const
    -> std::ostream&
  {
    using printer_t = rat::printer<ratexpset>;
    printer_t print(o, *this);
    print.format(format);
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    detail::transposer<ratexpset> tr{*this};
    return tr(v);
  }

#undef DEFINE

} // namespace vcsn
