#ifndef VCSN_WEIGHTS_R_HH
# define VCSN_WEIGHTS_R_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

# include <vcsn/misc/star_status.hh>
# include <vcsn/dyn/weightset.hh>

namespace vcsn
{
  class r: public dyn::detail::abstract_weightset
  {
  public:
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
        throw std::domain_error("r: star: invalid value: " + format(v));
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

    bool
    is_equal(const value_t l, const value_t r)
    {
      return l == r;
    }

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

    static value_t
    conv(std::istream& i)
    {
      value_t res;
      i >> res;
      if (i.fail())
        {
          char buf[256];
          i.getline (buf, sizeof buf);
          throw std::domain_error(std::string{"invalid double: "} + buf);
        }
      return res;
    }

    static value_t
    conv(const std::string& str)
    {
      std::istringstream i{str};
      return conv(i);
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

  /// The intersection of two weightsets.
  inline
  r intersection(const r&, const r&)
  {
    return {};
  }

  /// The union of two weightsets.
  inline
  r get_union(const r&, const r&)
  {
    return {};
  }
}

#endif // !VCSN_WEIGHTS_R_HH
