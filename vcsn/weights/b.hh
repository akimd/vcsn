#ifndef VCSN_WEIGHTS_B_HH
# define VCSN_WEIGHTS_B_HH

# include <cassert>
# include <ostream>
# include <stdexcept>
# include <string>
# include <vcsn/misc/star_status.hh>

namespace vcsn
{
  class b
  {
  public:
    static std::string sname()
    {
      return "b";
    }

    std::string vname(bool = true) const
    {
      return sname();
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
    static constexpr star_status_t star_status() { return star_status_t::STARABLE; }

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
      return o << format(v);
    }

    std::string
    format(const value_t v) const
    {
      return v ? "1" : "0";
    }
  };
}

#endif // !VCSN_WEIGHTS_B_HH
