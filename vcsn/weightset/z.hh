#pragma once

#include <ostream>
#include <string>

#include <vcsn/core/join.hh>
#include <vcsn/misc/functional.hh> // hash_value
#include <vcsn/misc/math.hh> // gcd
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/fwd.hh>

namespace vcsn
{
  namespace detail
  {
  class z_impl
  {
  public:
    using self_t = z;

    static symbol sname()
    {
      static auto res = symbol{"z"};
      return res;
    }

    /// Build from the description in \a is.
    static z make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = int;

    static value_t
    zero()
    {
      return 0;
    }

    static value_t
    one()
    {
      return 1;
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
      return detail::gcd(l, r);
    }

    value_t
    rgcd(const value_t l, const value_t r) const
    {
      return lgcd(l, r);
    }

    value_t
    rdiv(const value_t l, const value_t r) const
    {
      require(!is_zero(r), *this, "div: division by zero");
      require(!(l % r),
              *this, "div: invalid division: ", l, '/', r);
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
      if (is_zero(v))
        return one();
      else
        raise_not_starrable(*this, v);
    }

    constexpr static bool is_special(value_t)
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
    static bool less(value_t lhs, value_t rhs)
    {
      return lhs < rhs;
    }

    static constexpr bool is_commutative() { return true; }
    static constexpr bool is_idempotent() { return false; }
    static constexpr bool has_lightening_weights() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::NON_STARRABLE; }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    static size_t hash(value_t v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_t, value_t v)
    {
      return v;
    }

    static value_t
    conv(b, b::value_t v)
    {
      // Conversion from bool to int.
      return v;
    }

    value_t
    conv(std::istream& is, bool = true) const
    {
      int res;
      if (is >> res)
        return res;
      else
        vcsn::fail_reading(is, *this, ": invalid value");
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
          o << "\\mathbb{Z}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "Z";
          break;
        case format::utf8:
          o << "ℤ";
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(b, z);
    VCSN_JOIN_SIMPLE(z, z);
  }

}
