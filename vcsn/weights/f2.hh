#ifndef VCSN_WEIGHTS_F2_HH
# define VCSN_WEIGHTS_F2_HH

# include <cassert>
# include <ostream>
# include <stdexcept>
# include <string>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  class f2
  {
  public:
    using self_type = f2;

    static std::string sname()
    {
      return "f2";
    }

    std::string vname(bool = true) const
    {
      return sname();
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
    add(const value_t l, const value_t r)
    {
      return l ^ r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l && r;
    }

    static value_t
    star(const value_t v)
    {
      if (!v)
        return true;
      else
        throw std::domain_error("f2: star: invalid value: 1");
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
      return !v;
    }

    static bool
    is_one(const value_t v)
    {
      return v;
    }

    static constexpr bool is_commutative_semiring() { return true; }

    static constexpr bool show_one() { return false; }

    static constexpr
    star_status_t star_status() { return star_status_t::NON_STARRABLE; }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t
    conv(b, b::value_t v)
    {
      return v ? one() : zero();
    }

    static value_t
    conv(std::istream& stream)
    {
      int i;
      if ((stream >> i) && (i == 0 || i == 1))
        return i;
      else
        vcsn::fail_reading(stream, sname() + ": invalid value");
    }

    static std::ostream&
    print(std::ostream& o, const value_t v)
    {
      return o << format(v);
    }

    static std::string
    format(const value_t v)
    {
      return v ? "1" : "0";
    }
  };

  VCSN_WEIGHTS_BINARY(f2, f2, f2);

  VCSN_WEIGHTS_BINARY(b, f2, f2);
  VCSN_WEIGHTS_BINARY(f2, b, f2);
}

#endif // !VCSN_WEIGHTS_F2_HH
