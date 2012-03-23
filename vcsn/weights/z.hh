#ifndef VCSN_WEIGHTS_Z_HH
# define VCSN_WEIGHTS_Z_HH

# include <string>

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

    bool
    show_unit() const
    {
      return false;
    }

    value_t
    conv(const std::string &str) const
    {
      return atoi(str.c_str());
    }

  };
}

#endif // VCSN_WEIGHTS_Z_HH
