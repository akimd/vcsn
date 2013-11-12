#ifndef VCSN_WEIGHTS_Q_HH
# define VCSN_WEIGHTS_Q_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh> // eat
# include <vcsn/weights/fwd.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{

  class q
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
      void reduce()
      {
        int gc = gcd(abs(num), den);
        num /= gc;
        den /= gc;
      }

      int num;
      unsigned int den;
    };

    static unsigned int abs(int a)
    {
      return a < 0 ? -a : a;
    }

    // Greatest common divisor.
    ATTRIBUTE_PURE
    static unsigned int gcd(unsigned int a, unsigned int b)
    {
      while (b)
      {
        unsigned int t = a;
        a = b;
        b = t % b;
      }
      return a;
    }

    // Lowest common multiple
    ATTRIBUTE_PURE
    static unsigned int lcm(unsigned int a, unsigned int b)
    {
      return a / gcd(a, b) * b;
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
      value_t res{l.num * int (cm / l.den) + r.num * int (cm / r.den), cm};
      res.reduce();
      return res;
    }

    static value_t mul(const value_t l, const value_t r)
    {
      value_t res{l.num * r.num, l.den * r.den};
      res.reduce();
      return res;
    }

    static value_t star(const value_t v)
    {
      // Bad casting when v.den is too big
      if (abs(v.num) < v.den)
        return {int(v.den), v.den - v.num};
      else
        throw std::domain_error(sname() + ": star: invalid value: " + format(v));
    }

    static bool is_zero(const value_t v)
    {
      return v.num == 0;
    }

    static bool is_one(const value_t v)
    {
      // FIXME: all the values should be normalized.
      return 0 < v.num && static_cast<unsigned int>(v.num) == v.den;
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

    static constexpr bool is_commutative_semiring() { return true; }

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

    static value_t
    conv(self_type, value_t in)
    {
      return in;
    }

    static value_t
    conv(b, b::value_t in)
    {
      // Conversion from bool to q.
      return {in, 1};
    }

    static value_t
    conv(std::istream& i)
    {
      // FIXME: this routine could use some improvements...
      value_t res;
      i >> res.num;
      if (i.fail())
      {
        char buf[256];
        i.getline(buf, sizeof buf);
        throw std::domain_error(sname() + ": invalid value: " + str_escape(buf));
      }
      if (i.peek() == std::char_traits<char>::eof())
        return value_t{res.num, 1};
      if (i.get() != '/' || i.fail())
      {
        char buf[256];
        i.getline(buf, sizeof buf);
        throw std::domain_error(sname() + ": invalid value: " + str_escape(buf));
      }
      i >> res.den;
      if (i.fail())
      {
        char buf[256];
        i.getline(buf, sizeof buf);
        throw std::domain_error(sname() + ": invalid value: " + str_escape(buf));
      }
      unsigned int g = gcd(abs(res.num), res.den);
      res.num /= int(g);
      res.den /= g;
      return res;
    }

    static std::ostream&
    print(std::ostream& o, const value_t v)
    {
      // FIXME: Used to check "den == 0"!!!  Curently we accept "1/0".
      o << v.num;
      if (v.den != 1)
        o << '/' << v.den;
      return o;
    }

    static std::string
    format(const value_t v)
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }
  };

  VCSN_WEIGHTS_BINARY(q, q, q);


  VCSN_WEIGHTS_BINARY(b, q, q);
  VCSN_WEIGHTS_BINARY(q, b, q);
}

#endif // !VCSN_WEIGHTS_Q_HH
