#ifndef VCSN_WEIGHTSET_R_HH
# define VCSN_WEIGHTSET_R_HH

# include <string>
# include <ostream>

# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weightset/fwd.hh>
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/q.hh>
# include <vcsn/weightset/z.hh>
# include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class r_impl
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
    sub(const value_t l, const value_t r)
    {
      return l - r;
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return l * r;
    }

    static value_t
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      return l / r;
    }

    static value_t
    ldiv(const value_t l, const value_t r)
    {
      return rdiv(r, l);
    }

    value_t
    star(const value_t v) const
    {
      if (-1 < v && v < 1)
        return 1/(1-v);
      else
        raise(sname(), ": star: invalid value: ", format(*this, v));
    }

    constexpr static bool is_special(value_t)
    {
      return false;
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

    static size_t hash(value_t v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t
    conv(q, q::value_t v)
    {
      return value_t(v.num) / value_t(v.den);
    }

    static value_t
    conv(z, z::value_t v)
    {
      return v;
    }

    static value_t
    conv(b, b::value_t v)
    {
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
    print(const value_t v, std::ostream& o,
          const std::string& = "text")
    {
      return o << v;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      if (format == "latex")
        o << "\\mathbb{R}";
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };
  }

  VCSN_WEIGHTS_BINARY(r, r, r);

  VCSN_WEIGHTS_BINARY(q, r, r);
  VCSN_WEIGHTS_BINARY(r, q, r);

  VCSN_WEIGHTS_BINARY(z, r, r);
  VCSN_WEIGHTS_BINARY(r, z, r);

  VCSN_WEIGHTS_BINARY(b, r, r);
  VCSN_WEIGHTS_BINARY(r, b, r);
}

#endif // !VCSN_WEIGHTSET_R_HH
