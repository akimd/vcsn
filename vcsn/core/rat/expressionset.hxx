#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/algos/fwd.hh> // is-valid
#include <vcsn/core/rat/copy.hh>
#include <vcsn/core/rat/less.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/core/rat/hash.hh>
#include <vcsn/core/rat/transpose.hh>
#include <vcsn/dyn/algos.hh> // dyn::read_expression
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/expressionset.hh> // dyn::make_expressionset
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{

  namespace rat
  {

  template <typename Context>
  expressionset_impl<Context>::expressionset_impl(const context_t& ctx,
                                          identities_t identities)
    : ctx_(ctx)
    , identities_(identities)
  {
    require_weightset_commutativity();
  }

  template <typename Context>
  void expressionset_impl<Context>::require_weightset_commutativity() const
  {
    require(identities_ != identities_t::series
            || weightset_t_of<Context>::is_commutative(),
            "series (currently) depend on weightset commutativity");
  }

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  expressionset_impl<Context>

  DEFINE::sname()
    -> symbol
  {
    return symbol("expressionset<" + context_t::sname() + '>');
  }


  DEFINE::make(std::istream& is)
    -> expressionset<Context>
  {
    // name is, for instance, "expressionset<lal_char(abcd), z>(trivial)".
    eat(is, "expressionset<");
    auto ctx = Context::make(is);
    eat(is, '>');
    identities_t ids = rat::identities::trivial;
    if (is.peek() == '(')
      {
        eat(is, '(');
        is >> ids;
        eat(is, ')');
      }
    return {ctx, ids};
  }

  DEFINE::open(bool o) const
    -> bool
  {
    return this->labelset()->open(o);
  }

  DEFINE::context() const -> const context_t&
  {
    return ctx_;
  }

  DEFINE::identities() const -> identities_t
  {
    return identities_;
  }

  DEFINE::is_series() const -> bool
  {
    return identities_ == identities_t::series;
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
  template <typename expressionset_impl<Context>::type_t Type>
  inline
  auto
  expressionset_impl<Context>::gather_(values_t& res, value_t v) const
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
  template <typename expressionset_impl<Context>::type_t Type>
  inline
  auto
  expressionset_impl<Context>::gather_(value_t l, value_t r) const
    -> values_t
  {
    values_t res;
    gather_<Type>(res, l);
    gather_<Type>(res, r);
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
    else if (is_series())
      res = add_nonzero_series_(l, r);
    else
      res = std::make_shared<sum_t>(gather_<type_t::sum>(l, r));
    return res;
  }

  DEFINE::less_ignoring_weight_(value_t l, value_t r) const
    -> bool
  {
    return less(unwrap_possible_lweight_(l), unwrap_possible_lweight_(r));
  }

  DEFINE::remove_from_sum_series_(values_t addends,
                                  typename values_t::iterator i) const
    -> value_t
  {
    switch (addends.size())
      {
      case 0:
        assert(false); // Sum node with zero addends.
      case 1:
        return zero();
      case 2:
        addends.erase(i);
        return addends[0];
      default:
        addends.erase(i);
        return std::make_shared<sum_t>(std::move(addends));
      };
  }

  DEFINE::insert_in_sum_series_(const sum_t& addends, value_t r) const
    -> value_t
  {
    // We have to clone the addends (actually, their shared_ptr's)
    // into something we can modify.
    values_t copy = addends.subs();
    assert(copy.size() > 0);

    // Find the right spot where to insert r.
    const auto& ws = weightset();
    auto rw = possibly_implicit_lweight_(r);
    auto rn = unwrap_possible_lweight_(r);
    auto closure =
      [this] (value_t l, value_t r)
      {
        return less_ignoring_weight_(l, r);
      };
    const auto i = std::lower_bound(copy.begin(), copy.end(), rn, closure);
    if (i != copy.end()
        && equal(unwrap_possible_lweight_(*i), rn))
      {
        // An addend alraedy exists whose un-left-weighted value is rn.
        const weight_t& iw = possibly_implicit_lweight_(*i);
        const weight_t w = ws->add(rw, iw);
        if (ws->is_zero(w))
          return remove_from_sum_series_(std::move(copy), i);
        else
          *i = lmul(w, rn);
      }
    else
      copy.insert(i, r);

    return std::make_shared<sum_t>(std::move(copy));
  }

  DEFINE::merge_sum_series_(const sum_t& addends1, value_t aa2) const
    -> value_t
  {
    value_t res = aa2;
    for (const auto& e: addends1)
      res = add_nonzero_series_(res, e);
    return res;
  }

  DEFINE::add_nonzero_series_(value_t l, value_t r) const
    -> value_t
  {
    assert(! is_zero(r));
    if (l->type() == type_t::sum)
      {
        const auto ls = down_pointer_cast<const sum_t>(l);

        if (r->type() == type_t::sum)
          return merge_sum_series_(*ls, r);
        else
          return insert_in_sum_series_(*ls, r);
      }
    else if (r->type() == type_t::sum)
      return add_nonzero_series_(r, l);
    else
      {
        // Neither argument is a sum.
        auto ls = std::make_shared<sum_t>(values_t{l}); // Not in normal form.
        return insert_in_sum_series_(*ls, r);
      }
  }

  DEFINE::type_ignoring_lweight_(value_t e) const
    -> type_t
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
    if (is_series())
      return mul_series_(l, r);
    else
      return mul_expressions_(l, r);
  }

  DEFINE::mul_expressions_(value_t l, value_t r) const
    -> value_t
  {
    return mul_(l, r, false);
  }

  DEFINE::mul_series_(value_t l, value_t r) const
    -> value_t
  {
    return mul_(l, r, true);
  }

  DEFINE::mul_(value_t l, value_t r, bool series) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // U_K: E.(<k>1) ⇒ E<k>, subsuming T: E.1 = E.
    else if (type_ignoring_lweight_(r) == type_t::one)
      res = rmul(l, possibly_implicit_lweight_(r));
    // U_K: (<k>1).E ⇒ <k>E, subsuming T: 1.E = E.
    else if (type_ignoring_lweight_(l) == type_t::one)
      res = lmul(possibly_implicit_lweight_(l), r);
    // END: Trivial Identity
    else if (series) // Different from is_series().
      res = nontrivial_mul_series_(l, r);
    else
      res = nontrivial_mul_expressions_(l, r);
    return res;
  }

    DEFINE::is_unweighted_nonsum_(value_t v) const
    -> bool
    {
      assert(v->type() != type_t::lweight);
      // Of course we can assume v's subterms to be well-formed according
      // to our invariants: for example zero won't occur within a product.
      switch (v->type())
        {
        case type_t::sum:
          return false;
        default:
          return true;
        }
    }

  DEFINE::is_nonsum_(value_t v) const
    -> bool
  {
    return is_unweighted_nonsum_(unwrap_possible_lweight_(v));
  }

  DEFINE::mul_atoms_(const label_t& a, const label_t& b) const
    -> value_t
  {
    return mul_atoms_(a, b, typename is_law<Context>::type{});
  }

  DEFINE::mul_atoms_(const label_t& a, const label_t& b, std::true_type) const
    -> value_t
  {
    return atom(labelset()->mul(a, b));
  }

  DEFINE::mul_atoms_(const label_t& a, const label_t& b, std::false_type) const
    -> value_t
  {
    return std::make_shared<prod_t>(values_t{atom(a), atom(b)});
  }

  DEFINE::mul_unweighted_nontrivial_products_(value_t a, value_t b) const
    -> value_t
  {
    assert(! is_zero(a));
    assert(! is_zero(b));
    assert(! is_one(a));
    assert(! is_one(b));
    assert(! std::dynamic_pointer_cast<const lweight_t>(a));
    assert(! std::dynamic_pointer_cast<const lweight_t>(b));

    // The result is a product holding a's factors followed by b's
    // factors, with one exception: if the last factors of a can be
    // combined with the first factor of b then the two have to be
    // merged in the result.
    return concat(a, b);
  }

  DEFINE::mul_products_(value_t a, value_t b) const
    -> value_t
  {
    assert(! is_zero(a));
    assert(! is_zero(b));
    value_t na = unwrap_possible_lweight_(a), nb = unwrap_possible_lweight_(b);

    if (na->type() == type_t::one)
      return lmul(possibly_implicit_lweight_(a), b);
    else if (nb->type() == type_t::one)
      return lmul(weightset()->mul(possibly_implicit_lweight_(a),
                                   possibly_implicit_lweight_(b)),
                  na);
    else
      return lmul(weightset()->mul(possibly_implicit_lweight_(a),
                                   possibly_implicit_lweight_(b)),
                  mul_unweighted_nontrivial_products_(na, nb));
  }

  DEFINE::nontrivial_mul_expressions_(value_t l, value_t r) const
    -> value_t
  {
    return std::make_shared<prod_t>(gather_<type_t::prod>(l, r));
  }

  DEFINE::nontrivial_mul_series_(value_t l, value_t r) const
    -> value_t
  {
    // Compute the result by distributing product over sum.  We have
    // to use add rather than just build a vector in order, since the
    // addend order in the result will not follow the order in l.
    const auto& lt = type_ignoring_lweight_(l), rt = type_ignoring_lweight_(r);
    value_t res = zero();
    if (lt == type_t::sum)
      // l is a sum, and r might be as well.
      for (const auto& la: *down_pointer_cast<const sum_t>(l))
        res = add(res, mul(la, r));
    else if (rt == type_t::sum)
      // r is a sum, l is not.
      for (const auto& ra: *down_pointer_cast<const sum_t>(r))
        res = add(res, mul(l, ra));
    // Neither l nor r is a sum.
    else if (is_nonsum_(l) && is_nonsum_(r))
      return mul_products_(l, r);
    else
      {
        weight_t lw = possibly_implicit_lweight_(l)
          , rw = possibly_implicit_lweight_(r);
        value_t nl = unwrap_possible_lweight_(l)
          , nr = unwrap_possible_lweight_(r);
        return lmul(weightset()->mul(lw, rw),
                    std::make_shared<prod_t>(gather_<type_t::prod>(nl, nr)));
      }
     return res;
   }

  DEFINE::conjunction(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity: 0&E = 0.
    if (l->type() == type_t::zero)
      res = l;
    // Trivial Identity: E&0 = 0.
    else if (r->type() == type_t::zero)
      res = r;
    // <k>1&<h>1 = <kh>1.
    else if (type_ignoring_lweight_(l) == type_t::one
             && type_ignoring_lweight_(r) == type_t::one)
      res = lmul(weightset()->mul(possibly_implicit_lweight_(l),
                                  possibly_implicit_lweight_(r)),
                 one());
    // <k>a&<h>a = <kh>a.  <k>a&<h>b = 0.
    else if (type_ignoring_lweight_(l) == type_t::atom
             && type_ignoring_lweight_(r) == type_t::atom)
      {
        auto lhs =
          down_pointer_cast<const atom_t>(unwrap_possible_lweight_(l))->value();
        auto rhs =
          down_pointer_cast<const atom_t>(unwrap_possible_lweight_(r))->value();
        if (labelset()->equal(lhs, rhs))
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
      res = std::make_shared<conjunction_t>(gather_<type_t::conjunction>(l, r));
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
      res = std::make_shared<ldiv_t>(values_t{l, r});
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
      res = std::make_shared<shuffle_t>(gather_<type_t::shuffle>(l, r));
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
    return mul_expressions_(l, r);
  }

  // Concatenation when LAW.
  DEFINE::concat_(value_t l, value_t r, std::true_type) const
    -> value_t
  {
    // concat((ab).c, d.(ef)) = (ab).(cd).(ef).
    //
    // Store (ab) in expression, then concat(c, d) if c and d are atoms,
    // otherwise c then d, then (ef).
    if ((l->type() == type_t::atom || l->type() == type_t::prod)
        && (r->type() == type_t::atom || r->type() == type_t::prod))
      {
        // Left-hand sides.
        values_t ls;
        gather_<type_t::prod>(ls, l);
        // Right-hand sides.
        values_t rs;
        gather_<type_t::prod>(rs, r);

        if (ls.back()->type() == type_t::atom
            && rs.front()->type() == type_t::atom)
          {
            auto lhs = std::dynamic_pointer_cast<const atom_t>(ls.back());
            auto rhs = std::dynamic_pointer_cast<const atom_t>(rs.front());
            ls.back() = atom(labelset()->mul(lhs->value(), rhs->value()));
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
      return mul_expressions_(l, r);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    if (e->type() == type_t::zero)
      // Trivial one
      // (0)* == 1
      return one();
    else
      {
        value_t res = std::make_shared<star_t>(e);
        require(!is_series() || is_valid(*this, res),
                "star argument ", e, " not starrable");
        return res;
      }
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
    // a{T} = a, (abc){T} = cba.
    else if (auto l = std::dynamic_pointer_cast<const atom_t>(e))
      res = atom(labelset()->transpose(l->value()));
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
    // Trivial identities <k>0 => 0, <1>E => E.
    if (e->type() == type_t::zero || weightset()->is_one(w))
      return e;
    // Trivial identity: <0>E => 0.
    else if (weightset()->is_zero(w))
      return zero();
    // Trivial identity: <k>(<h>E) => <kh>E.
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lmul(weightset()->mul(w, lw->weight()), lw->sub());
    // General case: <k>E.
    else if (is_series() && e->type() == type_t::sum)
      {
        const auto& s = down_pointer_cast<const sum_t>(e);
        // We can build the result faster by emplace_back'ing addends without
        // passing thru add; the order will be the same as in *ss.
        values_t addends;
        for (auto& a: *s)
          addends.emplace_back(lmul(w, a));
        return std::make_shared<sum_t>(std::move(addends));
      }
    else
      return std::make_shared<lweight_t>(w, e);
  }

  DEFINE::rmul(value_t e, const weight_t& w) const
    -> value_t
  {
    // Trivial identity: E<0> => 0.
    if (weightset()->is_zero(w))
      return zero();
    // Trivial identity: E<1> => E.
    else if (weightset()->is_one(w))
      return e;
    else if (e->is_leaf())
      // Can only have left weights and lmul takes care of normalization.
      return lmul(w, e);
    // Trivial identity: (<k>E)<h> => <k>(E<h>).
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lmul(lw->weight(), rmul(lw->sub(), w));
    // Trivial identity: (E<k>)<h> => E<kh>.
    else if (auto rw = std::dynamic_pointer_cast<const rweight_t>(e))
      return rmul(rw->sub(), weightset()->mul(rw->weight(), w));
    // Series: distribute rmul on sums.
    else if (is_series() && e->type() == type_t::sum)
      {
        const auto& s = down_pointer_cast<const sum_t>(e);
        // Differently from the lmul case here the order of addends in
        // the result may be different from the order in *ss, so we
        // have to use add.
        value_t res = zero();
        for (auto& a: *s)
          res = add(res, rmul(a, w));
        return res;
      }
    // General case: E<k>.
    else
      return std::make_shared<rweight_t>(w, e);
  }

  /*----------------------------------.
  | expressionset as a WeightSet itself.  |
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

  DEFINE::less(value_t lhs, value_t rhs)
    -> bool
  {
    size<expressionset_impl> sizer;
    size_t l = sizer(lhs), r = sizer(rhs);

    if (l < r)
      return true;
    else if (l > r)
      return false;
    else
      {
        using less_t = rat::less<expressionset_impl>;
        less_t lt;
        return lt(lhs, rhs);
      }
  }

  DEFINE::equal(value_t lhs, value_t rhs)
    -> bool
  {
    return ! less(lhs, rhs) && ! less(rhs, lhs);
  }

  DEFINE::hash(const value_t& v)
    -> size_t
  {
    rat::hash<expressionset_impl> hasher;
    return hasher(v);
  }

  DEFINE::conv(self_type rs, value_t v) const
    -> value_t
  {
    if (identities() == rs.identities())
      return v;
    else
      return copy(rs, *this, v);
  }

  template <typename Context>
  template <typename GenSet>
  inline
  auto
  expressionset_impl<Context>::conv(const letterset<GenSet>& ls,
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

  DEFINE::conv(const zmin& ws, typename zmin::value_t v) const
    -> value_t
  {
    return lmul(weightset()->conv(ws, v), one());
  }

  template <typename Context>
  template <typename Ctx2>
  inline
  auto
  expressionset_impl<Context>::conv
  (const expressionset_impl<Ctx2>& rs,
   typename expressionset_impl<Ctx2>::value_t r) const
    -> value_t
  {
    return copy(rs, *this, r);
  }

  DEFINE::conv(std::istream& is) const
    -> value_t
  {
    auto dynres = dyn::read_expression(dyn::make_expressionset(*this), is);
    const auto& res = dynres->template as<expressionset_impl>();
    return res.expression();
  }

  DEFINE::print(const value_t v, std::ostream& o,
                const std::string& format) const
    -> std::ostream&
  {
    using printer_t = printer<expressionset_impl>;
    printer_t print(*this, o);
    print.format(format);
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    detail::transposer<expressionset_impl> tr{*this};
    return tr(v);
  }

  template <typename Context>
  template <typename... Args>
  inline
  auto
  expressionset_impl<Context>::letter_class(Args&&... args) const
    -> value_t
  {
    return letter_class_<labelset_t>(std::forward<Args>(args)...,
                                   std::is_same<labelset_t, vcsn::oneset>{});
  }

  template <typename Context>
  template <typename LabelSet_>
  inline
  auto
  expressionset_impl<Context>::letter_class_
    (std::set<std::pair<typename LabelSet_::letter_t,
                        typename LabelSet_::letter_t>> ccs,
     bool accept,
     std::false_type) const
    -> value_t
  {
    value_t res = zero();
    auto gens = labelset()->genset();
    // FIXME: This piece of code screams for factoring.  Yet, I want
    // to avoid useless costs such as building a empty/full set of
    // letters for [^].

    // [a-c].
    if (accept)
      for (const auto& cc: ccs)
        {
          auto i = std::find(std::begin(gens), std::end(gens), cc.first);
          auto end = std::find(i, std::end(gens), cc.second);
          require(end != std::end(gens),
                  "invalid letter interval: ",
                  to_string(*labelset(), labelset()->value(std::get<0>(cc))),
                  '-',
                  to_string(*labelset(), labelset()->value(std::get<1>(cc))));
          for (end = std::next(end); i != end; ++i)
            res = add(res, atom(labelset()->value(*i)));
        }
    // [^].
    else if (ccs.empty())
      for (auto l: gens)
        res = add(res, atom(labelset()->value(l)));
    // [^a-z].
    else
      {
        // Match the letters that are in no interval.
        std::set<typename LabelSet_::letter_t> accepted;
        for (const auto& cc: ccs)
          {
            auto i = std::find(std::begin(gens), std::end(gens), cc.first);
            auto end = std::find(i, std::end(gens), cc.second);
            require(end != std::end(gens),
                    "invalid letter interval: ",
                    to_string(*labelset(), labelset()->value(std::get<0>(cc))),
                    '-',
                    to_string(*labelset(), labelset()->value(std::get<1>(cc))));
            for (end = std::next(end); i != end; ++i)
              accepted.emplace(*i);
          }
        for (auto c: gens)
          if (!has(accepted, c))
            res = add(res, atom(labelset()->value(c)));
      }
    require(!is_zero(res),
            "invalid empty letter class");
    return res;
  }

  template <typename Context>
  template <typename LabelSet_, typename... Args>
  inline
  auto
  expressionset_impl<Context>::letter_class_(const Args&&...,
                                       std::true_type) const
    -> value_t
  {
    return one();
  }

#undef DEFINE

} // namespace rat
} // namespace vcsn
