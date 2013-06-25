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

    using value_t = double;

    value_t
    zero() const
    {
      return 0.;
    }

    value_t
    one() const
    {
      return 1.;
    }

    value_t
    add(const value_t l, const value_t r) const
    {
      return l + r;
    }

    value_t
    mul(const value_t l, const value_t r) const
    {
      return l * r;
    }

    value_t
    star(const value_t v) const
    {
      if (-1 < v && v < 1)
        return 1/(1-v);
      else
        throw std::domain_error("r: star: invalid value: " + format(v));
    }

    bool
    is_zero(const value_t v) const
    {
      return v == 0;
    }

    bool
    is_one(const value_t v) const
    {
      return v == 1;
    }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    value_t
    abs(const value_t v) const
    {
      return v < 0 ? -v : v;
    }

    value_t
    transpose(const value_t v) const
    {
      return v;
    }

    value_t
    conv(std::istream& i) const
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

    value_t
    conv(const std::string& str) const
    {
      std::istringstream i{str};
      return conv(i);
    }

    std::ostream&
    print(std::ostream& o, const value_t v) const
    {
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
}

#endif // !VCSN_WEIGHTS_R_HH
