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
    static std::string sname()
    {
      return "b";
    }

    using value_t = bool;

    value_t
    zero() const
    {
      return false;
    }

    value_t
    unit() const
    {
      return true;
    }

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
    star(const value_t) const
    {
      return unit();
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

    static constexpr bool show_unit() { return false; }
    static constexpr bool is_positive_semiring() { return true; }

    value_t
    transpose(const value_t v) const
    {
      return v;
    }

    value_t
    conv(const std::string& str) const
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

#endif // !VCSN_WEIGHTS_B_HH
