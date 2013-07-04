#ifndef VCSN_WEIGHTS_Q_HH
# define VCSN_WEIGHTS_Q_HH

# include <string>
# include <ostream>
# include <stdexcept>
# include <boost/lexical_cast.hpp>
# include <sstream>

# include <vcsn/misc/star_status.hh>
# include <vcsn/dyn/weightset.hh>
# include <vcsn/misc/attributes.hh>


namespace vcsn
{

  class q: public dyn::detail::abstract_weightset
  {
  public:
    static std::string sname()
    {
      return "q";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    struct value_t
    {
      int num;
      unsigned int den;
    };

    unsigned int abs(int a) const
    {
      return a < 0 ? -a : a;
    }

    // Highest Common factor
    ATTRIBUTE_PURE
    unsigned int gcd(unsigned int a, unsigned int b) const
    {
      while (b)
      {
	unsigned int t = a;
	a = b; 
	b = t % b;
      }
      return a;
    }

    // Lowest common multiple
    ATTRIBUTE_PURE
    unsigned int lcm(unsigned int a, unsigned int b) const
    {
      return a / gcd(a, b) * b;
    }

    value_t
    zero() const
    {
      return value_t{0, 1};
    }

    value_t
    one() const
    {
      return value_t{1, 1};
    }

    value_t
    add(const value_t l, const value_t r) const
    {
      value_t res;
      unsigned int cm = lcm(l.den, abs(r.den));
      res.num = l.num * (cm / l.den);
      res.num += r.num * (cm / r.den);
      res.den = cm;
      return res;
    }

    value_t
    mul(const value_t l, const value_t r) const
    {
      value_t res = value_t{l.num * r.num, l.den * r.den};
      unsigned int g = gcd(abs(res.num), res.den);
      res.num /= int(g);
      res.den /= g;
      return res;
    }

    value_t
    star(const value_t v) const
    {
      // Bad casting when v.den is too big 
      if (abs(v.num) < v.den)
      {
	unsigned int res_den = v.den - v.num;
	int res_num = v.den;
	return value_t{res_num, res_den};
      }
      else
        throw std::domain_error("r: star: invalid value: " + format(v));
    }

    bool
    is_zero(const value_t v) const
    {
      return v.num == 0;
    }

    bool
    is_one(const value_t v) const
    {
      return v.num > 0 && static_cast<unsigned int>(v.num) == v.den;
    }

    bool is_equal(const value_t l, const value_t r) 
    {
      return l.num == r.num && l.den == l.den;
    }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    value_t
    abs(const value_t v) const
    {
      return v.num < 0 ? (value_t{-v.num, v.den}) : v;
    }

    value_t
    transpose(const value_t v) const
    {
      return v;
    }

    value_t
    conv(std::istream& i) const
    {
      value_t res;
      i >> res.num;
      if (i.fail())
      {
	char buf[256];
	i.getline(buf, sizeof buf);
	throw std::domain_error(std::string{"invalid rational: "} + buf);
      }
      if (i.peek() == std::char_traits<char>::eof())
	return value_t{res.num, 1};
      if (i.get() != '/' || i.fail())
      {
	char buf[256];
	i.getline(buf, sizeof buf);
	throw std::domain_error(std::string{"invalid rational: "} + buf);
      }
      i >> res.den;
      if (i.fail())
      {
	char buf[256];
	i.getline(buf, sizeof buf);
	throw std::domain_error(std::string{"invalid rational: "} + buf);
      }
      unsigned int g = gcd(abs(res.num), res.den);		
      res.num /= int(g);
      res.den /= g;
      return res;
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
      if (v.den == 0)
	return o << 0;
      if (v.den == 1)
	return o << v.num;
      return o << v.num << "/" << v.den;
    }

    std::string
    format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }
  };
}

#endif // !VCSN_WEIGHTS_Q_HH
