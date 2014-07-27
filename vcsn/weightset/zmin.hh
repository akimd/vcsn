#ifndef VCSN_WEIGHTSET_ZMIN_HH
# define VCSN_WEIGHTSET_ZMIN_HH

# include <limits>
# include <ostream>
# include <string>
# include <utility>

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
  class zmin_impl
  {
  public:
    using self_type = zmin;

    static std::string sname()
    {
      return "zmin";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    /// Build from the description in \a is.
    static zmin make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = int;

    static value_t
    add(const value_t l, const value_t r)
    {
      return std::min(l, r);
    }

    static value_t
    mul(const value_t l, const value_t r)
    {
      return (is_zero(l) || is_zero(r) ? zero()
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
        raise(sname(), ": star: invalid value: ", format(*this, v));
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

    static constexpr bool show_one() { return true; }
    static constexpr star_status_t star_status() { return star_status_t::TOPS; }

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
    conv(b, b::value_t v)
    {
      return v ? one() : zero();
    }

    static value_t
    conv(std::istream& stream)
    {
      switch (int i = stream.peek())
        {
        case 'o':
          stream.ignore();
          if ((i = stream.get()) == 'o')
            return zero();
          else
            throw std::domain_error(sname() + ": invalid value: o" + str_escape(i));
        default:
          if (stream >> i)
            return i;
          else
            vcsn::fail_reading(stream, sname() + ": invalid value");
        }
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

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        o << "\\mathbb{Z}_{min}";
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };
  }

  VCSN_WEIGHTS_BINARY(zmin, zmin, zmin);

  VCSN_WEIGHTS_BINARY(b, zmin, zmin);
  VCSN_WEIGHTS_BINARY(zmin, b, zmin);
}

#endif // !VCSN_WEIGHTSET_ZMIN_HH
