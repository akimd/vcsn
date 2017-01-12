#pragma once

#include <cmath>
#include <iostream>
#include <string>

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
      return std::numeric_limits<value_t>::infinity();
    }

    /// The neutral element for the log semiring is 0
    static value_t one()
    {
      return 0;
    }

    static value_t
    min()
    {
      return std::numeric_limits<value_t>::min();
    }

    static value_t
    max()
    {
      return std::numeric_limits<value_t>::max();
    }

    static value_t add(const value_t l, const value_t r)
    {
      // https://lingpipe-blog.com/2009/06/25/log-sum-of-exponentials/
      auto diff = l - r;
      if (0 < diff)
        return r - std::log1p(std::exp(-diff));
      else if (diff <= 0)
        return l - std::log1p(std::exp(diff));
      else // NaN
        return zero();
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

    value_t
    rdivide(const value_t l, const value_t r) const
    {
      require(!is_zero(r), *this, ": div: division by zero");
      return is_zero(l) ? l : l - r;
    }

    value_t
    ldivide(const value_t l, const value_t r) const
    {
      return rdivide(r, l);
    }

    value_t star(const value_t v) const
    {
      if (v < 0)
        raise_not_starrable(*this, v);
      return std::log1p(-std::exp(-v));
    }

    /// Three-way comparison between \a l and \a r.
    static int compare(const value_t l, const value_t r)
    {
      // Beware of infinity.
      if (equal(l, r))
        return 0;
      else
        return l - r;
    }

    /// Whether \a l == \a r.
    static bool equal(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a l < \a r.
    static bool less(const value_t l, const value_t r)
    {
      return l < r;
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
    static constexpr bool has_lightening_weights() { return true; }

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

    value_t
    conv(std::istream& i, bool = true) const
    {
      value_t res;
      if (i.peek() == 'o')
        {
          eat(i, "oo");
          res = zero();
        }
      else if (! (i >> res))
        raise_invalid_value(*this, i);
      return res;
    }

    static std::ostream&
    print(const value_t v, std::ostream& o = std::cout,
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
        case format::utf8:
          o << "Log";
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

    // Random generation.
    template <typename RandomGenerator>
    class random_weight<log, RandomGenerator>
      : public random_weight_base<log, RandomGenerator>
    {
    public:
      using super_t = random_weight_base<log, RandomGenerator>;
      using value_t = typename super_t::weight_t;

      using super_t::super_t;

    private:
      value_t pick_value_() const
      {
        auto dis =
          std::uniform_real_distribution<log::value_t>(super_t::min_, super_t::max_);
        return dis(super_t::gen_);
      }
    };


    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(log, log);
  }// detail::
}
