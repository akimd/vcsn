#ifndef VCSN_WEIGHTS_Z_HH
# define VCSN_WEIGHTS_Z_HH

# include <ostream>
# include <string>

# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weights/b.hh>
# include <vcsn/weights/fwd.hh>

namespace vcsn
{
  class z
  {
  public:
    using self_type = z;

    static std::string sname()
    {
      return "z";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    /// Build from the description in \a is.
    static z make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = int;

    static value_t
    zero()
    {
      return 0;
    }

    static value_t
    one()
    {
      return 1;
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

    value_t
    star(const value_t v) const
    {
      if (is_zero(v))
        return one();
      else
        raise("z: star: invalid value: ", format(*this, v));
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
    static constexpr star_status_t star_status() { return star_status_t::NON_STARRABLE; }

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
      // Conversion from bool to int.
      return v;
    }

    static value_t
    conv(std::istream& stream)
    {
      int res;
      if (stream >> res)
        return res;
      else
        vcsn::fail_reading(stream, sname() + ": invalid value");
     }

    static std::ostream&
    print(std::ostream& o, const value_t v,
          const std::string& = "text")
    {
      return o << v;
    }
  };

  inline
  std::ostream&
  print_set(const z& ws,
            std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{Z}";
    else if (format == "text")
      o << ws.vname();
    else
      raise("invalid format: ", format);
    return o;
  }

  VCSN_WEIGHTS_BINARY(z, z, z);

  VCSN_WEIGHTS_BINARY(b, z, z);
  VCSN_WEIGHTS_BINARY(z, b, z);
}

#endif // !VCSN_WEIGHTS_Z_HH
