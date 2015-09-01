#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/lower_bound.hpp>

#include <vcsn/algos/fwd.hh> // is-valid
#include <vcsn/core/rat/copy.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/hash.hh>
#include <vcsn/core/rat/less.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/core/rat/transpose.hh>
#include <vcsn/dyn/algos.hh> // dyn::read_expression
#include <vcsn/dyn/context.hh> // dyn::make_context
#include <vcsn/dyn/fwd.hh>
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
                                                  identities_t ids)
    : ctx_(ctx)
    , ids_(ids)
  {
    require(!ids_.is_distributive() || weightset()->is_commutative(),
            "series (currently) requires a commutative weightset product");
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
    auto ids = identities_t{};
    if (is.peek() == '(')
      {
        eat(is, '(');
        is >> ids;
        eat(is, ')');
      }
    return {ctx, ids};
  }

  DEFINE::print_set(std::ostream& o, format fmt) const
    -> std::ostream&
  {
    if (fmt == format::latex)
      {
        o << "\\mathsf{";
        switch (identities().ids())
          {
          case identities_t::trivial:
            o << "RatE";
            break;
          case identities_t::distributive:
            o << "Series";
            break;
          default:
            assert(false);
          };
        o << "}[";
        context().print_set(o, fmt);
        o << ']';
      }
    else if (fmt == format::text)
      {
        if (identities().ids() == vcsn::rat::identities::distributive)
          {
            o << "seriesset<";
            context().print_set(o, fmt);
            o << '>';
          }
        else
          {
            o << "expressionset<";
            context().print_set(o, fmt);
            o << '>';
            if (identities() != vcsn::rat::identities{})
              o << '(' << identities() << ')';
          }
      }
    else
      raise("invalid format: ", fmt);
    return o;
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
    return ids_;
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
    else
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
    auto variadic = std::dynamic_pointer_cast<const variadic_t<Type>>(v);
    if (variadic && ids_.is_associative())
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
    if (ids_.is_associative())
      {
        gather_<Type>(res, l);
        gather_<Type>(res, r);
      }
    else
      {
        res.emplace_back(l);
        res.emplace_back(r);
      }
    return res;
  }

  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<sum_t>(l, r);

    // 0+E => E.
    else if (l->type() == type_t::zero)
      res = r;

    // E+0 => E.
    else if (r->type() == type_t::zero)
      res = l;

    // E+E => <2>E.
    else if (ids_.is_linear())
      res = add_linear_(l, r);

    else
      res = std::make_shared<sum_t>(gather_<type_t::sum>(l, r));
    return res;
  }

  DEFINE::add_linear_(const sum_t& s, value_t r) const
    -> value_t
  {
    auto res = values_t{};
    res.reserve(s.size() + 1);

    // Copy the strictly smaller part.
    auto i = boost::lower_bound(s, r, less_linear);
    res.insert(end(res), begin(s), i);

    if (i == end(s))
      res.emplace_back(r);
    else
      {
        if (less_linear(r, *i))
          res.emplace_back(r);
        else
          {
            auto w = weightset()->add(possibly_implicit_lweight_(*i),
                                      possibly_implicit_lweight_(r));
            if (!weightset()->is_zero(w))
              {
                auto l = unwrap_possible_lweight_(*i);
                res.emplace_back(lmul(w, l));
              }
            ++i;
          }
        res.insert(end(res), i, end(s));
      }

    return add_(std::move(res));
  }

  DEFINE::add_(values_t&& vs) const
    -> value_t
  {
    if (vs.size() == 0)
      return zero();
    else if (vs.size() == 1)
      return vs[0];
    else
      return std::make_shared<sum_t>(std::move(vs));
  }

  DEFINE::add_linear_(const sum_t& s1, const sum_t& s2) const
    -> value_t
  {
    auto res = values_t{};
    res.reserve(s1.size() + s2.size());
    // Merge two increasing lists.  Add weights of equal labels.
    using std::begin;
    using std::end;
    auto i1 = begin(s1), end1 = end(s1);
    auto i2 = begin(s2), end2 = end(s2);
    while (true)
      {
        if (i1 == end1)
          {
            res.insert(res.end(), i2, end2);
            break;
          }
        else if (i2 == end2)
          {
            res.insert(res.end(), i1, end1);
            break;
          }
        else if (less_linear(*i1, *i2))
          res.emplace_back(*i1++);
        else if (less_linear(*i2, *i1))
          res.emplace_back(*i2++);
        else
          {
            auto w = weightset()->add(possibly_implicit_lweight_(*i1),
                                      possibly_implicit_lweight_(*i2));
            if (!weightset()->is_zero(w))
              {
                auto l = unwrap_possible_lweight_(*i1);
                res.emplace_back(lmul(w, l));
              }
            ++i1;
            ++i2;
          }
      }
    return add_(std::move(res));
  }

  DEFINE::add_linear_(value_t l, value_t r) const
    -> value_t
  {
    assert(!is_zero(l));
    assert(!is_zero(r));
    value_t res = nullptr;
    if (auto ls = std::dynamic_pointer_cast<const sum_t>(l))
      {
        if (auto rs = std::dynamic_pointer_cast<const sum_t>(r))
          res = add_linear_(*ls, *rs);
        else
          res = add_linear_(*ls, r);
      }
    else if (auto rs = std::dynamic_pointer_cast<const sum_t>(r))
      res = add_linear_(*rs, l);
    else if (less_linear(l, r))
      res = std::make_shared<sum_t>(l, r);
    else if (less_linear(r, l))
      res = std::make_shared<sum_t>(r, l);
    else
      {
        auto w = weightset()->add(possibly_implicit_lweight_(l),
                                  possibly_implicit_lweight_(r));
        res = lmul(w, unwrap_possible_lweight_(l));
      }
    return res;
  }

  DEFINE::type_ignoring_lweight_(value_t e)
    -> type_t
  {
    return unwrap_possible_lweight_(e)->type();
  }

  DEFINE::possibly_implicit_lweight_(value_t e)
    -> weight_t
  {
    if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      return lw->weight();
    else
      return weightset_t::one();
  }

  DEFINE::unwrap_possible_lweight_(value_t e)
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

    if (!ids_)
      res = std::make_shared<prod_t>(l, r);

    // 0.E => 0.
    else if (l->type() == type_t::zero)
      res = l;

    // E.0 => 0.
    else if (r->type() == type_t::zero)
      res = r;

    // U_K: E.(<k>1) ⇒ E<k>, subsuming T: E.1 = E.
    else if (type_ignoring_lweight_(r) == type_t::one)
      res = rmul(l, possibly_implicit_lweight_(r));

    // U_K: (<k>1).E ⇒ <k>E, subsuming T: 1.E = E.
    else if (type_ignoring_lweight_(l) == type_t::one)
      res = lmul(possibly_implicit_lweight_(l), r);

    // (<k>E)(<h>F) => <kh>(EF).
    else if (ids_.is_linear() && weightset()->is_commutative()
             && (l->type() == type_t::lweight
                 || r->type() == type_t::lweight))
      {
        weight_t
          lw = possibly_implicit_lweight_(l),
          rw = possibly_implicit_lweight_(r);
        value_t
          nl = unwrap_possible_lweight_(l),
          nr = unwrap_possible_lweight_(r);
        res = lmul(weightset()->mul(lw, rw),
                   mul(nl, nr));
      }

    // (E+F)G => EG + FG.
    else if (ids_.is_distributive() && l->type() == type_t::sum)
      {
        res = zero();
        // l is a sum, and r might be as well.
        for (const auto& la: *down_pointer_cast<const sum_t>(l))
          res = add(res, mul(la, r));
      }

    // E(F+G) => EF + EG.
    else if (ids_.is_distributive() && r->type() == type_t::sum)
      {
        res = zero();
        // r is a sum, l is not.
        for (const auto& ra: *down_pointer_cast<const sum_t>(r))
          res = add(res, mul(l, ra));
      }

    else
      res = std::make_shared<prod_t>(gather_<type_t::prod>(l, r));
    return res;
  }

  DEFINE::conjunction(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<conjunction_t>(l, r);

    // 0&E => 0.
    else if (l->type() == type_t::zero)
      res = l;

    // E&0 => 0.
    else if (r->type() == type_t::zero)
      res = r;

    // <k>1&<h>1 => <kh>1.
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
    else if ((type_ignoring_lweight_(l) == type_t::one
              && type_ignoring_lweight_(r) == type_t::atom)
             || (type_ignoring_lweight_(l) == type_t::atom
                 && type_ignoring_lweight_(r) == type_t::one))
      res = zero();

    else
      res = std::make_shared<conjunction_t>(gather_<type_t::conjunction>(l, r));
    return res;
  }

  DEFINE::ldiv(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<ldiv_t>(l, r);

    // 0\E = 0{c}.
    else if (ids_ && l->type() == type_t::zero)
      res = complement(zero());

    // 1\E = E.
    else if (ids_ && l->type() == type_t::one)
      res = r;

    // E\0 = 0.
    else if (ids_ && r->type() == type_t::zero)
      res = r;

    else
      res = std::make_shared<ldiv_t>(l, r);
    return res;
  }

  DEFINE::rdiv(value_t l, value_t r) const
    -> value_t
  {
    // l/r = (r{T} {\} l{T}){T}.
    return transposition(ldiv(transposition(r), transposition(l)));
  }

  template <typename Context>
  template <typename... Value>
  auto expressionset_impl<Context>::tuple(Value&&... v) const
    -> value_t
  {
    auto ts = as_tupleset();
    auto t = ts.tuple(v...);
    if (ts.is_one(t))
      return one();
    else if (tuple_of_label<>::is_label(t))
      return atom(tuple_of_label<>::as_label(t));
    else
      // FIXME: when all letters, make it a (multitape) letter:
      // a|x -> 'a,x'.
      return std::make_shared<tuple_t>(std::forward<Value>(v)...);
  }

  DEFINE::infiltration(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<infiltration_t>(l, r);

    // 0&:E => 0.
    else if (ids_ && l->type() == type_t::zero)
      res = l;

    // E&:0 => 0.
    else if (ids_ && r->type() == type_t::zero)
      res = r;

    // 1&:E => E.
    else if (ids_ && l->type() == type_t::one)
      res = r;

    // E&:1 => E.
    else if (ids_ && r->type() == type_t::one)
      res = l;

    else
      res =
        std::make_shared<infiltration_t>(gather_<type_t::infiltration>(l, r));
    return res;
  }

  DEFINE::shuffle(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<shuffle_t>(l, r);

    // 0:E => 0.
    else if (ids_ && l->type() == type_t::zero)
      res = l;

    // E:0 => 0.
    else if (ids_ && r->type() == type_t::zero)
      res = r;

    // 1:E => E.
    else if (ids_ && l->type() == type_t::one)
      res = r;

    // E:1 => E.
    else if (ids_ && r->type() == type_t::one)
      res = l;

    else
      res = std::make_shared<shuffle_t>(gather_<type_t::shuffle>(l, r));
    return res;
  }

  /*-------.
  | power. |
  `-------*/

  DEFINE::power(value_t e, unsigned n) const
    -> value_t
  {
    value_t res = nullptr;
    // Given E the expression s.t. E{n} = (<k>a){n}.

    // E{0} => 1.
    if (n == 0)
      res = one();

    // E{1} => E.
    else if (n == 1)
      res = e;

    // \z{n} => \z.
    else if (ids_ && e->type() == type_t::zero)
      res = e;

    // Case: a == \e or a == <w>\e.
    else if (ids_ && type_ignoring_lweight_(e) == type_t::one)
      {
        weight_t w = possibly_implicit_lweight_(e);
        res = lmul(weightset()->power(w, n), one());
      }

    // Lweight in linear commutative: (<k>E){n} => <k{n}>(E{n}).
    else if (ids_.is_linear()
             && weightset()->is_commutative()
             && e->type() == type_t::lweight)
      {
        const auto& lw = down_pointer_cast<const lweight_t>(e);
        res = lmul(weightset()->power(lw->weight(), n),
                   power(lw->sub(), n));
      }

    // Sums in series: we have to distribute ([ab]{2} = aa+ab+ba+bb).
    else if (ids_.is_distributive() && e->type() == type_t::sum)
      {
        // FIXME: code duplication with weightset_mixin::power_.
        res = e;
        for (unsigned i = 1; i < n; ++i)
          res = mul(res, e);
      }

    // When associative, instead of repeated multiplication,
    // immediately create n copies of E.
    else if (ids_.is_associative())
      res = std::make_shared<prod_t>(n, e);

    // Default case: E{n} = ((..(EE)...)E.
    else
      {
        // FIXME: code duplication with weightset_mixin::power_.
        res = e;
        for (unsigned i = 1; i < n; ++i)
          res = mul(res, e);
      }

    return res;
  }

  DEFINE::concat(value_t l, value_t r) const
    -> value_t
  {
    // A static dispatch is needed, as the product of labels is not
    // available if not LAW.
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
    // concat((ab).<2>c, d.(ef)) = (ab).<2>(cd).(ef).
    //
    // Store (ab) in expression, then concat(<2>c, d) if c and d are
    // atoms, otherwise <2>c then d, then (ef).
    if ((type_ignoring_lweight_(l) == type_t::atom || l->type() == type_t::prod)
        && (r->type() == type_t::atom || r->type() == type_t::prod))
      {
        // Left-hand sides.
        values_t ls;
        gather_<type_t::prod>(ls, l);
        // Right-hand sides.
        values_t rs;
        gather_<type_t::prod>(rs, r);

        // FIXME: we should perform that "if" with the one above, and
        // enter this section only if we really are going to concat.
        // This would avoid the "else" clause.
        if (type_ignoring_lweight_(ls.back()) == type_t::atom
            && rs.front()->type() == type_t::atom)
          {
            // Fetch weight and atom of the last lhs.
            auto w = possibly_implicit_lweight_(ls.back());
            auto lhs
              = std::dynamic_pointer_cast<const atom_t>
              (unwrap_possible_lweight_(ls.back()));
            // Fetch atom of the first rhs.
            auto rhs = std::dynamic_pointer_cast<const atom_t>(rs.front());
            ls.back() = lmul(w,
                             atom(labelset()->mul(lhs->value(), rhs->value())));
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
    value_t res = nullptr;

    // \z* => 1.
    if (ids_ && e->type() == type_t::zero)
      res = one();

    else
      {
        res = std::make_shared<star_t>(e);
        require(!ids_.is_distributive() || is_valid(*this, res),
                "star argument ", e, " not starrable");
      }

    return res;
  }

  DEFINE::complement(value_t e) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<complement_t>(e);

    // The following identities make derived-term (<2>a)*{c} terminate.
    // (<k>E){c} => E{c}
    else if (auto w = std::dynamic_pointer_cast<const lweight_t>(e))
      res = complement(w->sub());

    // (E<k>){c} => E{c}.
    else if (auto w = std::dynamic_pointer_cast<const rweight_t>(e))
      res = complement(w->sub());

    else
      res = std::make_shared<complement_t>(e);

    return res;
  }

  DEFINE::transposition(value_t e) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<transposition_t>(e);

    // 0{T} => 0.
    else if (e->type() == type_t::zero)
      res = e;

    // 1{T} => 1.
    else if (e->type() == type_t::one)
      res = e;

    // a{T} => a, (abc){T} => cba.
    else if (auto l = std::dynamic_pointer_cast<const atom_t>(e))
      res = atom(labelset()->transpose(l->value()));

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
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<lweight_t>(w, e);

    // <k>0 => 0, <1>E => E.
    else if (e->type() == type_t::zero || weightset()->is_one(w))
      res = e;

    // <0>E => 0.
    else if (weightset()->is_zero(w))
      res = zero();

    // <k>(<h>E) => <kh>E.
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      res = lmul(weightset()->mul(w, lw->weight()), lw->sub());

    // Distributive: <k>(E+F) => <k>E + <k>F.
    else if (ids_.is_distributive() && e->type() == type_t::sum)
      {
        const auto& s = down_pointer_cast<const sum_t>(e);
        // We can build the result faster by emplace_back'ing addends without
        // passing thru add; the order will be the same as in *ss.
        values_t addends;
        for (const auto& a: *s)
          addends.emplace_back(lmul(w, a));
        res = std::make_shared<sum_t>(std::move(addends));
      }

    // General case: <k>E.
    else
      res = std::make_shared<lweight_t>(w, e);

    return res;
  }

  DEFINE::rmul(value_t e, const weight_t& w) const
    -> value_t
  {
    value_t res = nullptr;

    if (!ids_)
      res = std::make_shared<rweight_t>(w, e);

    // Linear identity: E<k> => <k>E.
    else if (ids_.is_linear() && weightset()->is_commutative())
      res = lmul(w, e);

    // Trivial identity: E<0> => 0.
    else if (weightset()->is_zero(w))
      res = zero();

    // Trivial identity: E<1> => E.
    else if (weightset()->is_one(w))
      res = e;

    else if (e->is_leaf())
      // Can only have left weights and lmul takes care of normalization.
      res = lmul(w, e);

    // Trivial identity: (<k>E)<h> => <k>(E<h>).
    else if (auto lw = std::dynamic_pointer_cast<const lweight_t>(e))
      res = lmul(lw->weight(), rmul(lw->sub(), w));

    // Trivial identity: (E<k>)<h> => E<kh>.
    else if (auto rw = std::dynamic_pointer_cast<const rweight_t>(e))
      res = rmul(rw->sub(), weightset()->mul(rw->weight(), w));

    // General case: E<k>.
    else
      res = std::make_shared<rweight_t>(w, e);

    return res;
  }

  /*--------------------------------------.
  | expressionset as a WeightSet itself.  |
  `--------------------------------------*/

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
    auto lt = rat::less<self_t>{};
    return lt(lhs, rhs);
  }

  DEFINE::less_linear(value_t lhs, value_t rhs)
    -> bool
  {
    return less(unwrap_possible_lweight_(lhs),
                unwrap_possible_lweight_(rhs));
  }

  DEFINE::equal(value_t lhs, value_t rhs)
    -> bool
  {
    return ! less(lhs, rhs) && ! less(rhs, lhs);
  }

  DEFINE::hash(const value_t& v)
    -> size_t
  {
    auto hasher = rat::hash<self_t>{};
    return hasher(v);
  }

  DEFINE::conv(const self_t& rs, value_t v) const
    -> value_t
  {
    if (ids_ == rs.ids_)
      return v;
    else
      return vcsn::rat::copy(rs, self(), v);
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
  expressionset_impl<Context>::conv(const expressionset<Ctx2>& rs,
                                    typename expressionset<Ctx2>::value_t r)
    const
    -> value_t
  {
    return vcsn::rat::copy(rs, self(), r);
  }

  DEFINE::conv(std::istream& is) const
    -> value_t
  {
    // Our expression parser is written in dyn::, so we get a
    // dyn::expression that we down_cast.
    auto dynres
      = dyn::read_expression(dyn::make_context(context()), identities(),
                             is);
    const auto& res = dynres->template as<self_t>();
    return res.expression();
  }

  DEFINE::print(const value_t v, std::ostream& o,
                format fmt) const
    -> std::ostream&
  {
    auto print = make_printer(self(), o);
    print.format(fmt);
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    auto tr = detail::transposer<self_t>{self()};
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
    using boost::range::find;
    value_t res = zero();
    auto gens = labelset()->genset();
    // FIXME: This piece of code screams for factoring.  Yet, I want
    // to avoid useless costs such as building a empty/full set of
    // letters for [^].

    // [a-c].
    if (accept)
      for (const auto& cc: ccs)
        {
          auto i = find(gens, cc.first);
          auto end = std::find(i, std::end(gens), cc.second);
          require(end != std::end(gens),
                  "invalid letter interval: ",
                  to_string(*labelset(), labelset()->value(std::get<0>(cc))),
                  '-',
                  to_string(*labelset(), labelset()->value(std::get<1>(cc))));
          for (++end; i != end; ++i)
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
            auto i = find(gens, cc.first);
            auto end = std::find(i, std::end(gens), cc.second);
            require(end != std::end(gens),
                    "invalid letter interval: ",
                    to_string(*labelset(), labelset()->value(std::get<0>(cc))),
                    '-',
                    to_string(*labelset(), labelset()->value(std::get<1>(cc))));
            for (++end; i != end; ++i)
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
