#ifndef VCSN_WEIGHTS_ZMIN_HH
# define VCSN_WEIGHTS_ZMIN_HH
# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>
# include <limits>
# include <utility>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/dyn/weightset.hh>
namespace vcsn
{
  class zmin: public dyn::detail::abstract_weightset
  {
  public:
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
    value_t
    add(const value_t l, const value_t r) const
    {
      return std::min(l, r);
    }
    value_t
    mul(const value_t l, const value_t r) const
    {
      return (is_zero(l) || is_zero(r) ? zero()
              : l + r);
    }
    value_t
    star(const value_t v) const
    {
      if (0 <= v)
        return one();
      else
        throw std::domain_error("zmin: star: invalid value: " + format(v));
    }
    value_t
    one() const
    {
      return 0;
    }
    value_t
    zero() const
    {
      return std::numeric_limits<value_t>::max();
    }
    bool
    is_one(const value_t v) const
    {
      return v == one();
    }
    bool
    is_zero(const value_t v) const
    {
      return v == zero();
    }
    static constexpr bool show_one() { return true; }
    static constexpr star_status_t star_status() { return star_status_t::TOPS; }
    value_t
    transpose(const value_t v) const
    {
      return v;
    }
    value_t
    conv(const std::string &str) const
    {
      if (str == "oo")
        return zero();
      try
        {
          return boost::lexical_cast<value_t>(str);
        }
      catch (std::bad_cast& e)
        {
          throw std::domain_error("invalid integer: " + str);
        }
    }
    std::ostream&
    print(std::ostream& o, const value_t v) const
    {
      if (is_zero(v))
        return o << "oo";
      return o << v;
    }
    std::string
    format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }
  };
  /// The intersection of two weightsets.
  inline
  zmin intersection(const zmin&, const zmin&)
  {
    return {};
  }
  /// The union of two weightsets.
  inline
  zmin get_union(const zmin&, const zmin&)
  {
    return {};
  }
}
#endif // !VCSN_WEIGHTS_ZMIN_HH