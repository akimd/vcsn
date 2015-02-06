#pragma once

#include <iostream>
#include <string>

#include <vcsn/core/join.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/hash.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star_status.hh>
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
    using self_type = b;

    static symbol sname()
    {
      static symbol res("b");
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

    static value_t
    lgcd(const value_t l, const value_t r)
    {
      require(!is_zero(l), sname(), ": lgcd: invalid lhs: zero");
      require(!is_zero(r), sname(), ": lgcd: invalid rhs: zero");
      return one();
    }

    static value_t
    rgcd(const value_t l, const value_t r)
    {
      return lgcd(l, r);
    }

    static value_t
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      return l;
    }

    static value_t
    ldiv(const value_t l, const value_t r)
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
    conv(self_type, const value_t v)
    {
      return v;
    }

    static value_t
    conv(std::istream& is)
    {
      int i;
      if (is >> i)
        {
          if (i == 0 || i == 1)
            return i;
          else
            fail_reading(is,
                         sname(), ": invalid value: ", std::to_string(i));
        }
      else
        fail_reading(is, sname(), ": invalid value");
    }

    static std::ostream&
    print(const value_t v, std::ostream& o,
          const std::string& format = "text")
    {
      if (format == "latex")
        o << (v ? "\\top" : "\\bot");
      else
        o << (v ? '1' : '0');
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        o << "\\mathbb{B}";
      else if (format == "text")
        o << sname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };

    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(b, b);
  } // detail::

}
