#ifndef VCSN_WEIGHTS_Z_HH
# define VCSN_WEIGHTS_Z_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

# include <vcsn/misc/star_status.hh>
# include <vcsn/dyn/weightset.hh>

namespace vcsn
{
  class z: public dyn::detail::abstract_weightset
  {
  public:
    static std::string sname()
    {
      return "z";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    using value_t = int;

    value_t
    zero() const
    {
      return 0;
    }

    value_t
    one() const
    {
      return 1;
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
      if (is_zero(v))
        return one();
      else
        throw std::domain_error("z: star: invalid value: " + format(v));
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
    static constexpr star_status_t star_status() { return star_status_t::NON_STARRABLE; }

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
          throw std::domain_error("invalid integer: " + str);
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

#endif // !VCSN_WEIGHTS_Z_HH
