#ifndef VCSN_WEIGHTSET_Q_HH
# define VCSN_WEIGHTSET_Q_HH

# include <string>
# include <ostream>

# include <vcsn/core/join.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/math.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh> // eat
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/fwd.hh>
# include <vcsn/weightset/weightset.hh>
# include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace detail
  {
  class q_impl
  {
  public:
    using self_type = q;

    static std::string sname()
    {
      return "q";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    /// Build from the description in \a is.
    static q make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    struct value_t
    {
      value_t(int n = 0, unsigned d = 1)
        : num(n)
        , den(d)
      {}

      /// Put it in normal form.
      value_t& reduce()
      {
        int gc = gcd(abs(num), den);
        num /= gc;
        den /= gc;
        return *this;
      }

      int num;
      unsigned int den;
    };

    static unsigned int abs(int a)
    {
      return a < 0 ? -a : a;
    }

    static value_t zero()
    {
      return value_t{0, 1};
    }

    static value_t one()
    {
      return value_t{1, 1};
    }

    static value_t add(const value_t l, const value_t r)
    {
      unsigned int cm = lcm(l.den, abs(r.den));
      return value_t{l.num * int (cm / l.den) + r.num * int (cm / r.den),
                     cm}.reduce();
    }

    static value_t sub(const value_t l, const value_t r)
    {
      unsigned int cm = lcm(l.den, abs(r.den));
      return value_t{l.num * int (cm / l.den) - r.num * int (cm / r.den),
                     cm}.reduce();
    }

    static value_t mul(const value_t l, const value_t r)
    {
      return value_t{l.num * r.num, l.den * r.den}.reduce();
    }

    static value_t
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      if (0 < r.num)
        return value_t{l.num * int(r.den), l.den * r.num}.reduce();
      else
        return value_t{-l.num * int(r.den), l.den * -r.num}.reduce();
    }

    static value_t
    ldiv(const value_t l, const value_t r)
    {
      return rdiv(r, l);
    }

    value_t star(const value_t v) const
    {
      // Bad casting when v.den is too big
      if (abs(v.num) < v.den)
        // No need to reduce: numerator and denominators are primes.
        return {int(v.den), v.den - v.num};
      else
        raise(sname(), ": star: invalid value: ", to_string(*this, v));
    }

    static bool is_special(const value_t) // C++11: cannot be constexpr.
    {
      return false;
    }

    static bool is_zero(const value_t v)
    {
      return v.num == 0;
    }

    static bool is_one(const value_t v)
    {
      // All values are normalized.
      return v.num == 1 && v.den == 1;
    }

    static bool equals(const value_t l, const value_t r)
    {
      return l.num == r.num && l.den == r.den;
    }

    /// Whether \a lhs < \a rhs.
    static bool less_than(value_t lhs, value_t rhs)
    {
      return lhs.num * rhs.den < rhs.num * lhs.den;
    }

    static constexpr bool is_commutative() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    static value_t
    abs(const value_t v)
    {
      return v.num < 0 ? (value_t{-v.num, v.den}) : v;
    }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    static size_t hash(value_t v)
    {
      size_t res = 0;
      std::hash_combine(res, hash_value(v.num));
      std::hash_combine(res, hash_value(v.den));
      return res;
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t
    conv(z, z::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(b, b::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(std::istream& i)
    {
      int num;
      if (! (i >> num))
        vcsn::fail_reading(i, sname() + ": invalid numerator");

      // If we have a slash after the numerator then we have a
      // denominator as well.
      char maybe_slash;
      if ((maybe_slash = i.peek()) != '/')
        return value_t{num, 1};
      vcsn::eat(i, '/');

      // operator>> with an istream and an unsigned int silently
      // mangles a negative number into its two's complement
      // representation as a positive number.
      if (i.peek() == '-')
        {
          num = - num;
          vcsn::eat(i, '-');
        }

      unsigned int den;
      if (i >> den)
        {
          // Make sure our rational respects our constraints.
          if (den == 0)
            throw std::domain_error(sname() + ": zero denominator");
          return value_t{num, den}.reduce();
        }
      else
        vcsn::fail_reading(i, sname() + ": invalid denominator");
    }

    static std::ostream&
    print(const value_t v, std::ostream& o,
          const std::string& format = "text")
    {
      if (format == "latex")
        {
          if (v.den == 1)
            o << v.num;
          else
            o << "\\frac{" << v.num << "}{" << v.den << '}';
        }
      else
        {
          o << v.num;
          if (v.den != 1)
            o << '/' << v.den;
        }
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        o << "\\mathbb{Q}";
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };

    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(b, q);
    VCSN_JOIN_SIMPLE(z, q);
    VCSN_JOIN_SIMPLE(q, q);
  }
}

#endif // !VCSN_WEIGHTSET_Q_HH
