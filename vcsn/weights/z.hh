#ifndef VCSN_WEIGHTS_Z_HH
# define VCSN_WEIGHTS_Z_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

namespace vcsn
{
  class z
  {
  public:
    typedef int value_t;

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
    unit() const
    {
      return 1;
    }

    value_t
    zero() const
    {
      return 0;
    }

    bool
    is_unit(const value_t v) const
    {
      return v == 1;
    }

    bool
    is_zero(const value_t v) const
    {
      return v == 0;
    }

    static constexpr bool show_unit() { return false; }
    static constexpr bool is_positive_semiring() { return false; }

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
      std::stringstream s;
      print(s, v);
      return s.str();
    }
  };
}

#endif // VCSN_WEIGHTS_Z_HH
