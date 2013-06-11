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
    star(const value_t v) const
    {
      if (!v)
        return true;
      else
        throw std::domain_error("f2: star: invalid value: 1");
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
    conv(std::istream& i) const
    {
      switch (char c = i.get())
        {
        case '0': return false;
        case '1': return true;
        default:
          throw std::domain_error("invalid F2: " + c);
        }
    }

    value_t
    conv(const std::string& str) const
    {
      std::istringstream i{str};
      return conv(i);
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

#endif // !VCSN_WEIGHTS_F2_HH
