#pragma once

#include <iostream>
#include <string>

#include <vcsn/core/join.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/functional.hh> // hash_value
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class b_impl
  {
  public:
    using self_t = b;

    static symbol sname()
    {
      static auto res = symbol{"b"};
      return res;
    }

    /// Build from the description in \a is.
    static b make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = bool;

    static value_t
    zero()
    {
      return false;
    }

    static value_t
    one()
    {
      return true;
    }

    static value_t
    add(const value_t l, const value_t r)
    {
      return l || r;
    }

    // This is highly debatable.  Was introduced to allow the division
    // of polynomials.  It should rather be handled there.
    static value_t
    sub(const value_t l, const value_t r)
    {
      return l && !r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l && r;
    }

    value_t
    lgcd(const value_t l, const value_t r) const
    {
      require(!is_zero(l), *this, ": lgcd: invalid lhs: zero");
      require(!is_zero(r), *this, ": lgcd: invalid rhs: zero");
      return one();
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
      return l;
    }

    value_t
    ldiv(const value_t l, const value_t r) const
    {
      return rdiv(r, l);
    }

    static value_t
    star(const value_t)
    {
      return one();
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

    constexpr static bool is_special(const value_t)
    {
      return false;
    }

    static bool
    is_zero(const value_t v)
    {
      return !v;
    }

    static bool
    is_one(const value_t v)
    {
      return v;
    }

    static constexpr bool is_commutative() { return true; }
    static constexpr bool is_idempotent() { return true; }
    static constexpr bool has_lightening_weights() { return false; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
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

    value_t
    conv(std::istream& is, bool = true) const
    {
      int i;
      if (is >> i)
        {
          require(i == 0 || i == 1,
                  *this, ": invalid value: ", i);
          return i;
        }
      else
        raise(*this, ": invalid value: ", is);
    }

    static std::ostream&
    print(const value_t v, std::ostream& o = std::cout,
          format fmt = {})
    {
      if (fmt == format::latex)
        o << (v ? "\\top" : "\\bot");
      else
        o << (v ? '1' : '0');
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathbb{B}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "B";
          break;
        case format::utf8:
          o << "𝔹";
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

    VCSN_JOIN_SIMPLE(b, b);
  } // detail::

}
