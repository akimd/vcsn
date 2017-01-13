#pragma once

#include <vcsn/misc/format.hh>
#include <vcsn/misc/functional.hh> // hash_value
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh> // eat
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Derived, typename Value>
    class min_plus_impl
    {
    public:
      using self_t = Derived;
      using value_t = Value;

      const self_t& self() const
      {
        return static_cast<const self_t&>(*this);
      }

      static value_t
      add(const value_t l, const value_t r)
      {
        return std::min(l, r);
      }

      static value_t
      mul(const value_t l, const value_t r)
      {
        return (is_zero(l) || is_zero(r)
                ? zero()
                : l + r);
      }

      value_t
      rdivide(const value_t l, const value_t r) const
      {
        require(!is_zero(r), self(), "div: division by zero");
        return is_zero(l) ? l : l - r;
      }

      value_t
      ldivide(const value_t l, const value_t r) const
      {
        return rdivide(r, l);
      }

      value_t
      star(const value_t v) const
      {
        if (0 <= v)
          return one();
        else
          raise_not_starrable(self(), v);
      }

      static value_t
      one()
      {
        return 0;
      }

      static value_t
      zero()
      {
        return std::numeric_limits<value_t>::max();
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

      /// Three-way comparison between \a l and \a r.
      static int
      compare(const value_t l, const value_t r)
      {
        return l - r;
      }

      /// Whether \a l == \a r.
      static bool
      equal(const value_t l, const value_t r)
      {
        return l == r;
      }

      /// Whether \a l < \a r.
      static bool
      less(value_t l, value_t r)
      {
        return l < r;
      }

      constexpr static bool
      is_special(value_t)
      {
        return false;
      }

      static bool
      is_zero(const value_t v)
      {
        return v == zero();
      }

      static bool
      is_one(const value_t v)
      {
        return v == one();
      }

      static constexpr bool is_commutative() { return true; }
      static constexpr bool is_idempotent() { return true; }

      static constexpr bool show_one() { return true; }
      static constexpr star_status_t star_status()
      {
        return star_status_t::TOPS;
      }

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
      conv(b, b::value_t v)
      {
        return v ? one() : zero();
      }

      value_t
      conv(std::istream& is, bool = true) const
      {
        value_t res;
        if (is.peek() == 'o')
          {
            is.ignore();
            int c = is.get();
            if (c == 'o')
              res = zero();
            else
              raise_invalid_value(self(), "o", c);
          }
        else if (!(is >> res))
          raise_invalid_value(self(), is);
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
    };
  }
}
