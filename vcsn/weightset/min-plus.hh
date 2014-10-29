#ifndef VCSN_WEIGHTSET_MIN_PLUS_HH
# define VCSN_WEIGHTSET_MIN_PLUS_HH

# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh> // eat
# include <vcsn/weightset/fwd.hh>
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Value>
    class min_plus_impl
    {
    public:
      using value_t = Value;

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

      static value_t
      rdiv(const value_t l, const value_t r)
      {
        require(!is_zero(r), "div: division by zero");
        return l - r;
      }

      static value_t
      ldiv(const value_t l, const value_t r)
      {
        return rdiv(r, l);
      }

      value_t
      star(const value_t v) const
      {
        if (0 <= v)
          return one();
        else
          // FIXME: sname.
          raise("star: invalid value: ", format(*this, v));
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

      static bool
      equals(const value_t l, const value_t r)
      {
        return l == r;
      }

      /// Whether \a lhs < \a rhs.
      static bool less_than(value_t lhs, value_t rhs)
      {
        return lhs < rhs;
      }

      constexpr static bool is_special(value_t)
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

      static value_t
      conv(std::istream& is)
      {
        value_t res;
        if (is.peek() == 'o')
          {
            is.ignore();
            char c = is.get();
            if (c == 'o')
              res = zero();
            else
              // FIXME: sname.
              throw std::domain_error("invalid value: o" + str_escape(c));
          }
        else if (!(is >> res))
          // FIXME: sname.
          vcsn::fail_reading(is, "invalid value");
        return res;
      }

      static std::ostream&
      print(const value_t v, std::ostream& o,
            const std::string& format = "text")
      {
        if (is_zero(v))
          return o << (format == "latex" ? "\\infty" : "oo");
        else
          return o << v;
      }
    };
  }
}

#endif // !VCSN_WEIGHTSET_MIN_PLUS_HH