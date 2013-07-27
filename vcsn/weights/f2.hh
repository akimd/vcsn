#ifndef VCSN_WEIGHTS_F2_HH
# define VCSN_WEIGHTS_F2_HH

# include <cassert>
# include <ostream>
# include <stdexcept>
# include <string>

# include <vcsn/dyn/weightset.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  class f2: public dyn::detail::abstract_weightset
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

    /// Build from the description in \a is.
    static f2 make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = bool;

    value_t
    zero() const
    {
      return false;
    }

    value_t
    one() const
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
    is_one(const value_t v) const
    {
      return v;
    }

    bool
    is_zero(const value_t v) const
    {
      return !v;
    }

    static constexpr bool show_one() { return false; }

    static constexpr
    star_status_t star_status() { return star_status_t::NON_STARRABLE; }

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
          throw std::domain_error("invalid F2: " + str_escape(c));
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
