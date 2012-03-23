#ifndef VCSN_WEIGHTS_B_HH
#define VCSN_WEIGHTS_B_HH

#include <string>
#include <cassert>

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

    bool
    conv(std::string& str) const
    {
      return str[0] != '0';
    }
  };
}

#endif // VCSN_WEIGHTS_B_HH
