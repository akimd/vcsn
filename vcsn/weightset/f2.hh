#ifndef VCSN_WEIGHTSETS_F2_HH
# define VCSN_WEIGHTSETS_F2_HH

# include <cassert>
# include <ostream>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class f2_impl
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
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      return l;
    }

    static value_t
    ldiv(const value_t l, const value_t r)
    {
      return rdiv(r, l);
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
    conv(std::istream& stream)
    {
      int i;
      if ((stream >> i) && (i == 0 || i == 1))
        return i;
      else
        vcsn::fail_reading(stream, sname() + ": invalid value");
    }

    static std::ostream&
    print(std::ostream& o, const value_t v,
          const std::string& = "text")
    {
      return o << (v ? '1' : '0');
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      if (format == "latex")
        o << "\\mathbb{F}_2";
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };
  }// detail::

  VCSN_WEIGHTS_BINARY(f2, f2, f2);
}

#endif // !VCSN_WEIGHTSETS_F2_HH
