#pragma once

#include <string>
#include <iostream>
#include <cmath>

#include <vcsn/core/join.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/functional.hh> // hash_value
#include <vcsn/misc/math.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh> // eat
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class log_impl
  {
  public:
    using self_t = log;
    using value_t = double;

    static symbol sname()
    {
      static auto res = symbol{"log"};
      return res;
    }

    /// Build from the description in \a is.
    static log make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    /// The zero for the log semiring is +oo
    static value_t zero()
    {
      return std::numeric_limits<value_t>::max();
    }

    /// The neutral element for the log semiring is 0
    static value_t one()
    {
      return 0;
    }

    static value_t add(const value_t l, const value_t r)
    {
      return - std::log(std::exp(-l) + std::exp(-r));
    }

    static value_t sub(const value_t l, const value_t r)
    {
      return - std::log(std::exp(-l) - std::exp(-r));
    }

    static value_t mul(const value_t l, const value_t r)
    {
      // We have to check if a member is +oo
      return is_zero(l) || is_zero(r) ? zero() : l + r;
    }

    static value_t
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      return is_zero(l) ? l : l - r;
    }

    static value_t
    ldiv(const value_t l, const value_t r)
    {
      return rdiv(r, l);
    }

    value_t star(const value_t v) const
    {
      if (v < 1)
        return std::log(1 - v);
      else
        // We use this for -oo
        return std::numeric_limits<value_t>::min();
    }

    static bool equal(const value_t l, const value_t r)
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

    static bool is_zero(const value_t v)
    {
      return v == zero();
    }

    static bool is_one(const value_t v)
    {
      return v == one();
    }

    static constexpr bool is_commutative() { return true; }
    static constexpr bool is_idempotent() { return false; }

    static constexpr bool show_one() { return true; }
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

    static value_t
    conv(std::istream& i, bool = true)
    {
      value_t res;
      if (i.peek() == 'o')
        {
          eat(i, "oo");
          res = zero();
        }
      else if (! (i >> res))
        raise(sname(), ": invalid value: ", i);
      return res;
    }

    static std::ostream&
    print(const value_t v, std::ostream& o,
          format fmt = {})
    {
      if (is_zero(v))
        return o << (fmt == format::latex ? "\\infty" : "oo");
      else
        return o << v;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathrm{Log}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "Log";
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

    VCSN_JOIN_SIMPLE(log, log);
  }// detail::
}
