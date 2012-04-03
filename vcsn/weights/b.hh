#ifndef VCSN_WEIGHTS_B_HH
# define VCSN_WEIGHTS_B_HH

# include <cassert>
# include <ostream>
# include <stdexcept>
# include <string>

namespace vcsn
{
  class b
  {
  public:
    typedef bool value_t;

    value_t
    add(const value_t l, const value_t r) const
    {
      return l || r;
    }

    value_t
    mul(const value_t l, const value_t r) const
    {
      return l && r;
    }

    value_t
    unit() const
    {
      return true;
    }

    value_t
    zero() const
    {
      return false;
    }

    bool
    is_unit(const value_t v) const
    {
      return v;
    }

    bool
    is_zero(const value_t v) const
    {
      return !v;
    }

    bool
    show_unit() const
    {
      return false;
    }

    value_t
    conv(std::string& str) const
    {
      if (str == "0")
        return false;
      else if (str == "1")
        return true;
      else
        throw std::domain_error("invalid Boolean: " + str);
    }

    std::ostream&
    print(std::ostream& o, const value_t v) const
    {
      return o << (v ? "0" : "1");
    }

    std::string
    format(const value_t v) const
    {
      return v ? "0" : "1";
    }
  };
}

#endif // VCSN_WEIGHTS_B_HH
