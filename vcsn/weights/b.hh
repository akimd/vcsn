#ifndef VCSN_WEIGHTS_B_HH
# define VCSN_WEIGHTS_B_HH

# include <string>
# include <ostream>
# include <cassert>

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
      if (str.size())
       {
         if ('0' == str[0])
           return false;
         else if ('1' == str[0])
           return true;
       }
      else
       {
         if (str == "true")
           return true;
         else if (str == "false")
           return false;
       }
      assert(false);
      return false;
    }

    std::ostream&
    print(std::ostream& o, const value_t v) const
    {
      return o << (v ? "true" : "false");
    }
  };
}

#endif // VCSN_WEIGHTS_B_HH
