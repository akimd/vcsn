#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/core/rat/copy.hh>
#include <vcsn/core/rat/less-than.hh>
#include <vcsn/core/rat/ratexp.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/core/rat/hash.hh>
#include <vcsn/core/rat/transpose.hh>
#include <vcsn/dyn/algos.hh> // dyn::read_ratexp_string
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/ratexpset.hh> // dyn::make_ratexpset
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  namespace rat
  {

  template <typename Context>
  ratexpset_impl<Context>::ratexpset_impl(const context_t& ctx)
    : ctx_(ctx)
  {}

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  ratexpset_impl<Context>

  DEFINE::sname()
    -> std::string
  {
    return "ratexpset<" + context_t::sname() + '>';
  }

  DEFINE::vname(bool full) const
    -> std::string
  {
    return "ratexpset<" + context().vname(full) + '>';
  }

  DEFINE::make(std::istream& is)
    -> ratexpset<Context>
  {
    // name is, for instance, "ratexpset<lal_char(abcd)_z>".
    eat(is, "ratexpset<");
    auto ctx = Context::make(is);
    eat(is, '>');
    return {ctx};
  }

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

  DEFINE::atom(const label_t& v)
    -> value_t
  {
    if (labelset_t::is_one(v))
      return one();
    return std::make_shared<atom_t>(v);
  }

  DEFINE::zero() const
    -> value_t
  {
    return std::make_shared<zero_t>();
  }

  DEFINE::one()
    -> value_t
  {
    return std::make_shared<one_t>();
  }

  template <typename Context>
  template <exp::type_t Type>
  inline
  auto
  ratexpset_impl<Context>::gather(ratexps_t& res, value_t v) const
    -> void
  {
    static bool binary = !! getenv("VCSN_BINARY");
    auto variadic = std::dynamic_pointer_cast<const variadic_t<Type>>(v);
    if (variadic && ! binary)
      res.insert(std::end(res), std::begin(*variadic), std::end(*variadic));
    else
      res.push_back(v);
  }

  template <typename Context>
  template <exp::type_t Type>
  inline
  auto
  ratexpset_impl<Context>::gather(value_t l, value_t r) const
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
    -> exp::type_t
  {
    return unwrap_possible_lweight_(e)->type();
  }

  DEFINE::possibly_implicit_lweight_(value_t e) const
    -> weight_t
  {
    if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lw->weight();
    else
      return weightset()->one();
  }

  DEFINE::unwrap_possible_lweight_(value_t e) const
    -> value_t
  {
    if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lw->sub();
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

  DEFINE::conjunction(value_t l, value_t r) const
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
      res = std::make_shared<conjunction_t>(gather<type_t::conjunction>(l, r));
    return res;
  }

  DEFINE::ldiv(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // 0\E = 0{c}.
    if (l->type() == type_t::zero)
      res = complement(zero());
    // 1\E = E.
    else if (l->type() == type_t::one)
      res = r;
    // E\0 = 0.
    else if (r->type() == type_t::zero)
      res = r;
    else
      res = std::make_shared<ldiv_t>(ratexps_t{l, r});
    return res;
  }

  DEFINE::rdiv(value_t l, value_t r) const
    -> value_t
  {
    // l/r = (r{T} {\} l{T}){T}.
    return transposition(ldiv(transposition(r), transposition(l)));
  }

  DEFINE::shuffle(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity.
    // E:0 = 0:E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // E:1 = 1:E = E.
    else if (l->type() == type_t::one)
      res = r;
    else if (r->type() == type_t::one)
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
    // concat((ab).c, d.(ef)) = (ab).(cd).(ef).
    //
    // Store (ab) in ratexp, then concat(c, d) if c and d are atoms,
    // otherwise c then d, then (ef).
    if ((l->type() == type_t::atom || l->type() == type_t::prod)
        && (r->type() == type_t::atom || r->type() == type_t::prod))
      {
        // Left-hand sides.
        ratexps_t ls;
        gather<type_t::prod>(ls, l);
        // Right-hand sides.
        ratexps_t rs;
        gather<type_t::prod>(rs, r);

        if (ls.back()->type() == type_t::atom
            && rs.front()->type() == type_t::atom)
          {
            auto lhs = std::dynamic_pointer_cast<const atom_t>(ls.back());
            auto rhs = std::dynamic_pointer_cast<const atom_t>(rs.front());
            ls.back() = atom(labelset()->concat(lhs->value(), rhs->value()));
            ls.insert(ls.end(), rs.begin() + 1, rs.end());
          }
        else
          ls.insert(ls.end(), rs.begin(), rs.end());
        if (ls.size() == 1)
          return ls.front();
        else
          return std::make_shared<prod_t>(ls);
      }
    else
      // Handle all the trivial identities.
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
    if (auto w = std::dynamic_pointer_cast<const lweight_t>(e))
      return complement(w->sub());
    else if (auto w = std::dynamic_pointer_cast<const rweight_t>(e))
      return complement(w->sub());
    else
      return std::make_shared<complement_t>(e);
  }

  DEFINE::transposition(value_t e) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity.
    // 0{T} = 0.
    if (e->type() == type_t::zero)
      res = e;
    // 1{T} = 1.
    else if (e->type() == type_t::one)
      res = e;
    // a{T} = a.
    else if (e->type() == type_t::atom)
      res = e;
    // END: Trivial Identity
    else
      res = std::make_shared<transposition_t>(e);
    return res;
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
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lmul(weightset()->mul(w, lw->weight()), lw->sub());
    // General case: <k>E.
    else
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
    // Trivial identity: (<k>E)<h> => <k>(E<h>).
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lmul(lw->weight(), rmul(lw->sub(), w));
    // Trivial identity: (E<k>)<h> => E<kh>.
    else if (auto rw = std::dynamic_pointer_cast<const rweight_t>(e))
      return rmul(rw->sub(), weightset()->mul(rw->weight(), w));
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

  DEFINE::is_one(value_t v)
    -> bool
  {
    return (v->type() == type_t::one);
  }

  DEFINE::less_than(value_t lhs, value_t rhs)
    -> bool
  {
    size<ratexpset_impl> sizer;
    size_t l = sizer(lhs), r = sizer(rhs);

    if (l < r)
      return true;
    else if (l > r)
      return false;
    else
      {
        using less_than_t = rat::less_than<ratexpset_impl>;
        less_than_t lt;
        return lt(lhs, rhs);
      }
  }

  DEFINE::equals(value_t lhs, value_t rhs)
    -> bool
  {
    return ! less_than(lhs, rhs) && ! less_than(rhs, lhs);
  }

  DEFINE::hash(const value_t& v)
    -> size_t
  {
    rat::hash<ratexpset_impl> hasher;
    return hasher(v);
  }

  DEFINE::conv(self_type, value_t v) const
    -> value_t
  {
    return v;
  }

  template <typename Context>
  template <typename GenSet>
  inline
  auto
  ratexpset_impl<Context>::conv(const letterset<GenSet>& ls,
                                typename letterset<GenSet>::value_t v) const
    -> value_t
  {
    return atom(labelset()->conv(ls, v));
  }

  DEFINE::conv(b, typename b::value_t v) const
    -> value_t
  {
    return v ? one() : zero();
  }

  DEFINE::conv(const z& ws, typename z::value_t v) const
    -> value_t
  {
    return lmul(weightset()->conv(ws, v), one());
  }

  DEFINE::conv(const q& ws, typename q::value_t v) const
    -> value_t
  {
    return lmul(weightset()->conv(ws, v), one());
  }

  DEFINE::conv(const r& ws, typename r::value_t v) const
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
  ratexpset_impl<Context>::conv(const ratexpset_impl<Ctx2>& ws,
                                typename ratexpset_impl<Ctx2>::value_t v) const
    -> value_t
  {
    return copy(ws, *this, v);
  }

  DEFINE::conv(std::istream& is) const
    -> value_t
  {
    auto dynres = dyn::read_ratexp(is, dyn::make_ratexpset(*this));
    const auto& res = dynres->template as<ratexpset_impl>();
    return res.ratexp();
  }

  DEFINE::print(std::ostream& o, const value_t v,
		const std::string& format) const
    -> std::ostream&
  {
    using printer_t = printer<ratexpset_impl>;
    printer_t print(o, *this);
    print.format(format);
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    detail::transposer<ratexpset_impl> tr{*this};
    return tr(v);
  }

  template <typename Context>
  template <typename... Args>
  inline
  auto
  ratexpset_impl<Context>::char_class(Args&&... args) const
    -> value_t
  {
    return char_class_<labelset_t>(std::forward<Args>(args)...,
                                   std::is_same<labelset_t, vcsn::oneset>{});
  }

  template <typename Context>
  template <typename LabelSet_>
  inline
  auto
  ratexpset_impl<Context>::char_class_(std::set<std::pair<typename LabelSet_::letter_t,
                                                          typename LabelSet_::letter_t>> ccs,
                                       bool accept,
                                       std::false_type) const
    -> value_t
  {
    value_t res = zero();
    auto gens = labelset()->genset();
    if (accept)
      {
        if (ccs.empty())
          for (auto l: gens)
            res = add(res, atom(labelset()->value(l)));
        else
          for (auto cc: ccs)
            {
              auto i = std::find(std::begin(gens), std::end(gens), cc.first);
              auto end = std::find(i, std::end(gens), cc.second);
              if (end != std::end(gens))
                for (end = std::next(end); i != end; ++i)
                  res = add(res, atom(labelset()->value(*i)));
            }
      }
    else
      {
        // Match the letters that are in no interval.
        std::set<typename LabelSet_::letter_t> accepted;
        for (auto cc: ccs)
            {
              auto i = std::find(std::begin(gens), std::end(gens), cc.first);
              auto end = std::find(i, std::end(gens), cc.second);
              if (end != std::end(gens))
                for (end = std::next(end); i != end; ++i)
                  accepted.emplace(*i);
            }
        for (auto c: gens)
          if (!has(accepted, c))
            res = add(res, atom(labelset()->value(c)));
      }
    return res;
  }

  template <typename Context>
  template <typename LabelSet_, typename... Args>
  inline
  auto
  ratexpset_impl<Context>::char_class_(const Args&&...,
                                       std::true_type) const
    -> value_t
  {
    return one();
  }

#undef DEFINE

} // namespace rat
} // namespace vcsn
