#ifndef VCSN_WEIGHTS_R_HH
# define VCSN_WEIGHTS_R_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weights/fwd.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  class r
  {
  public:
    using self_type = r;

    static std::string sname()
    {
      return "r";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    /// Build from the description in \a is.
    static r make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = double;

    static value_t
    zero()
    {
      return 0.;
    }

    static value_t
    one()
    {
      return 1.;
    }

    static value_t
    add(const value_t l, const value_t r)
    {
      return l + r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l * r;
    }

    static value_t
    star(const value_t v)
    {
      if (-1 < v && v < 1)
        return 1/(1-v);
      else
        throw std::domain_error(sname() + ": star: invalid value: " + format(v));
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
    equals(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a lhs < \a rhs.
    static bool less_than(value_t lhs, value_t rhs)
    {
      return lhs < rhs;
    }

    static constexpr bool is_commutative_semiring() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    static value_t
    abs(const value_t v)
    {
      return v < 0 ? -v : v;
    }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    template <typename From>
    static value_t
    conv(typename From::value_t v)
    {
      return conv(std::declval<From>(), v);
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t
    conv(b, b::value_t v)
    {
      // Conversion from bool to double.
      return v;
    }

    static value_t
    conv(std::istream& i)
    {
      value_t res;
      if (i >> res)
        return res;
      else
        vcsn::fail_reading(i, sname() + ": invalid value");
    }

    static std::ostream&
    print(std::ostream& o, const value_t v)
    {
      return o << v;
    }

    static std::string
    format(const value_t v)
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }
  };

  VCSN_WEIGHTS_BINARY(r, r, r);
  VCSN_WEIGHTS_BINARY(b, r, r);
  VCSN_WEIGHTS_BINARY(r, b, r);
}

#endif // !VCSN_WEIGHTS_R_HH
