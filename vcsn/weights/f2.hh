#ifndef VCSN_WEIGHTS_F2_HH
# define VCSN_WEIGHTS_F2_HH

# include <cassert>
# include <ostream>
# include <stdexcept>
# include <string>
# include <vcsn/misc/star_status.hh>

namespace vcsn
{
  class f2
  {
  public:
    static std::string sname()
    {
      return "f2";
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
      return l ^ r;
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
    static constexpr star_status_t star_status() { return star_status_t::NON_STARABLE; }

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
        throw std::domain_error("invalid F2: " + str);
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

#endif // !VCSN_WEIGHTS_F2_HH
