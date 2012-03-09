#ifndef VCSN_CORE_POLY_HH
#define VCSN_CORE_POLY_HH

#include <map>
#include <iostream>

namespace vcsn
{
  template <class Alphabet, class WeightSet>
  struct polynomial
  {
  public:
    typedef typename Alphabet::word_t word_t;
    typedef typename WeightSet::value_t weight_t;

    typedef std::map<word_t, weight_t> value_t;

    polynomial(Alphabet a, WeightSet ws = WeightSet())
      : a_(a), ws_(ws)
    {
      unit_[a_.identity()] = ws_.unit();
    }

    value_t&
    assoc(value_t& v, const word_t& w, const weight_t& k) const
    {
      v[w] = k;
      return v;
    }

    value_t&
    add_assoc(value_t& v, const word_t& w, const weight_t& k) const
    {
      auto i = v.find(w);
      if (i == v.end())
	assoc(v, w, k);
      else
	i->second = ws_.add(i->second, k);
      return v;
    }

    value_t
    add(const value_t& l, const value_t& r) const
    {
      value_t p = l;
      for (auto& i : r)
	add_assoc(p, i.first, i.second);
      return p;
    }

    value_t
    mul(const value_t& l, const value_t& r) const
    {
      value_t p;
      for (auto i: l)
	for (auto j: r)
	  add_assoc(p,
		    a_.concat(i.first, j.first),
		    ws_.mul(i.second, j.second));
      return p;
    }

    const value_t&
    unit() const
    {
      return unit_;
    }

    bool
    is_unit(const value_t& v) const
    {
      if (v.size() != 1)
	return false;
      auto i = v.find(a_.identity());
      if (i == v.end())
	return false;
      return ws_.is_unit(i->second);
    }

    const value_t&
    zero() const
    {
      return zero_;
    }

    bool
    is_zero(const value_t& v) const
    {
      return v.empty();
    }

    std::ostream&
    output(std::ostream& out, const value_t& v)
    {
      bool first = true;
      bool show_unit = ws_.show_unit();

      for (auto& i: v)
	{
	  if (!first)
	    out << " + ";
	  first = false;

	  if (show_unit || !ws_.is_unit(i.second))
	    out << "{" << i.second << "}";
	  a_.output(out, i.first);
	}

      if (first)
	out << "\\z";

      return out;
    }

  private:
    Alphabet a_;
    WeightSet ws_;
    value_t unit_;
    value_t zero_;
  };

}

#endif // VCSN_CORE_POLY_HH
