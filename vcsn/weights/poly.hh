#ifndef VCSN_WEIGHTS_POLY_HH
# define VCSN_WEIGHTS_POLY_HH

# include <map>
# include <iostream>
# include <sstream>

namespace vcsn
{
  template <class Context>
  struct polynomials
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;

    using word_t = typename genset_t::word_t;
    using weight_t = typename weightset_t::value_t;

    using value_t = std::map<word_t, weight_t>;

    polynomials(const genset_t& gs, const weightset_t& ws)
      : gs_(gs), ws_(ws)
    {
      unit_[gs_.identity()] = ws_.unit();
    }

    const genset_t&  genset() const  { return gs_; }
    const weightset_t& weightset() const { return ws_; }

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
		    gs_.concat(i.first, j.first),
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
      auto i = v.find(gs_.identity());
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

    static constexpr bool show_unit() { return true; }
    static constexpr bool is_positive_semiring()
    {
      return weightset_t::is_positive_semiring();
    }

    std::ostream&
    print(std::ostream& out, const value_t& v) const
    {
      bool first = true;
      bool show_unit = ws_.show_unit();

      for (auto& i: v)
	{
	  if (!first)
	    out << " + ";
	  first = false;

	  if (show_unit || !ws_.is_unit(i.second))
	    {
	      out << "{";
	      ws_.print(out, i.second) << "}";
	    }
	  gs_.output(out, i.first);
	}

      if (first)
	out << "\\z";

      return out;
    }

    std::string
    format(const value_t& v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }

  private:
    const genset_t& gs_;
    const weightset_t& ws_;
    value_t zero_;
    value_t unit_;
  };

}

#endif // !VCSN_WEIGHTS_POLY_HH
