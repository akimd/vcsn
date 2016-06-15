#pragma once

#include <string>
#include <ostream>

#include <vcsn/core/join.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/functional.hh> // hash_value
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/q.hh>
#include <vcsn/weightset/qmp.hh>
#include <vcsn/weightset/z.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class r_impl
  {
  public:
    using self_t = r;

    static symbol sname()
    {
      static auto res = symbol{"r"};
      return res;
    }

    /// Build from the description in \a is.
    static r make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = double;

    static value_t
    zero()
    {
      return 0.;
    }

    static value_t
    one()
    {
      return 1.;
    }

    static value_t
    add(const value_t l, const value_t r)
    {
      return l + r;
    }

    static value_t
    sub(const value_t l, const value_t r)
    {
      return l - r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l * r;
    }

    value_t
    lgcd(const value_t l, const value_t r) const
    {
      require(!is_zero(l), *this, ": lgcd: invalid lhs: zero");
      require(!is_zero(r), *this, ": lgcd: invalid rhs: zero");
      return l;
    }

    value_t
    rgcd(const value_t l, const value_t r) const
    {
      return lgcd(l, r);
    }

    value_t
    rdiv(const value_t l, const value_t r) const
    {
      require(!is_zero(r), *this, ": div: division by zero");
      return l / r;
    }

    value_t
    ldiv(const value_t l, const value_t r) const
    {
      return rdiv(r, l);
    }

    value_t
    star(const value_t v) const
    {
      if (-1 < v && v < 1)
        return 1/(1-v);
      else
        raise_not_starrable(*this, v);
    }

    constexpr static bool is_special(const value_t)
    {
      return false;
    }

    static bool
    is_zero(const value_t v)
    {
      return v == 0;
    }

    static bool
    is_one(const value_t v)
    {
      return v == 1;
    }

    static bool
    equal(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a lhs < \a rhs.
    static bool less(const value_t lhs, const value_t rhs)
    {
      return lhs < rhs;
    }

    static constexpr bool is_commutative() { return true; }
    static constexpr bool is_idempotent() { return false; }
    static constexpr bool has_lightening_weights() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    static value_t
    abs(const value_t v)
    {
      return v < 0 ? -v : v;
    }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    static size_t hash(const value_t v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_t, const value_t v)
    {
      return v;
    }

    static value_t
    conv(q, const q::value_t v)
    {
      return value_t(v.num) / value_t(v.den);
    }

    static value_t
    conv(z, const z::value_t v)
    {
      return v;
    }

    static value_t
    conv(b, const b::value_t v)
    {
      return v;
    }

    value_t
    conv(std::istream& i, bool = true) const
    {
      value_t res;
      if (i >> res)
        return res;
      else
        raise(*this, ": invalid value: ", i);
    }

    static std::ostream&
    print(const value_t v, std::ostream& o = std::cout,
          format = {})
    {
      return o << v;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathbb{R}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "R";
          break;
        case format::utf8:
          o << "ℝ";
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

    VCSN_JOIN_SIMPLE(b, r);
    VCSN_JOIN_SIMPLE(z, r);
    VCSN_JOIN_SIMPLE(q, r);
    VCSN_JOIN_SIMPLE(qmp, r);
    VCSN_JOIN_SIMPLE(r, r);
  }

}
