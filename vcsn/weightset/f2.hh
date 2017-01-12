#pragma once

#include <cassert>
#include <ostream>

#include <vcsn/core/join.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/fwd.hh> // f2
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class f2_impl
  {
  public:
    using self_t = f2;

    static symbol sname()
    {
      static auto res = symbol{"f2"};
      return res;
    }

    /// Build from the description in \a is.
    static f2 make(std::istream& is)
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
    min()
    {
      return false;
    }

    static value_t
    max()
    {
      return true;
    }

    static value_t
    add(const value_t l, const value_t r)
    {
      return l ^ r;
    }

    static value_t
    sub(const value_t l, const value_t r)
    {
      return l ^ r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l && r;
    }

    value_t
    rdivide(const value_t l, const value_t r) const
    {
      require(!is_zero(r), *this, ": div: division by zero");
      return l;
    }

    value_t
    ldivide(const value_t l, const value_t r) const
    {
      return rdivide(r, l);
    }

    value_t
    star(const value_t v) const
    {
      if (v == 0)
        return one();
      else
        raise_not_starrable(*this, v);
    }

    /// Three-way comparison between \a l and \a r.
    static int compare(const value_t l, const value_t r)
    {
      return l - r;
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a lhs < \a rhs.
    static bool less(value_t l, value_t r)
    {
      return l < r;
    }

    constexpr static bool is_special(value_t)
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
    static constexpr bool is_idempotent() { return false; }
    static constexpr bool has_lightening_weights() { return false; }

    static constexpr bool show_one() { return false; }

    static constexpr
    star_status_t star_status() { return star_status_t::NON_STARRABLE; }

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

    value_t
    conv(std::istream& is, bool = true) const
    {
      int i;
      if (is >> i)
        {
          if (i != 0 && i != 1)
            raise_invalid_value(*this, i);
          return i;
        }
      else
        raise_invalid_value(*this, is);
    }

    static std::ostream&
    print(const value_t v, std::ostream& o = std::cout,
          format = {})
    {
      return o << (v ? '1' : '0');
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathbb{F}_2";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "F2";
          break;
        case format::utf8:
          o << "𝔽₂";
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

    /// Random generation.
    template <typename RandomGenerator>
    class random_weight<f2, RandomGenerator>
      : public random_weight_base<f2, RandomGenerator>
    {
    public:
      using super_t = random_weight_base<f2, RandomGenerator>;
      using value_t = typename super_t::weight_t;

      using super_t::super_t;

    private:
      value_t pick_value_() const
      {
        auto dis = std::uniform_int_distribution<>(super_t::min_, super_t::max_);
        if (dis(super_t::gen_))
          return super_t::ws_.zero();
        else
          return super_t::ws_.one();
      }
    };

    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(f2, f2);
  }// detail::
}
