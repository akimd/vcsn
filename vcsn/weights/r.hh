#ifndef VCSN_WEIGHTS_R_HH
# define VCSN_WEIGHTS_R_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>
# include <vcsn/misc/star_status.hh>

namespace vcsn
{
  class r
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
    unit() const
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
    is_unit(const value_t v) const
    {
      return v == 1;
    }

    static constexpr bool show_unit() { return false; }
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
    conv(const std::string &str) const
    {
      try
        {
          return boost::lexical_cast<value_t>(str);
        }
      catch (std::bad_cast& e)
        {
          throw std::domain_error("invalid double: " + str);
        }
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
