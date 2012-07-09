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

    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using word_t = typename genset_t::word_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<word_t, weight_t>;

    polynomials(const context_t& ctx)
      : ctx_(ctx)
    {
      unit_[genset()->identity()] = weightset()->unit();
    }

    const context_t& context() const { return ctx_; }
    const genset_ptr& genset() const { return ctx_.genset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

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
	i->second = weightset()->add(i->second, k);
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
		    genset()->concat(i.first, j.first),
		    weightset()->mul(i.second, j.second));
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
      auto i = v.find(genset()->identity());
      if (i == v.end())
	return false;
      return weightset()->is_unit(i->second);
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
      bool show_unit = weightset()->show_unit();

      for (const auto& i: v)
	{
	  if (!first)
	    out << " + ";
	  first = false;

	  if (show_unit || !weightset()->is_unit(i.second))
	    {
	      out << "{";
	      weightset()->print(out, i.second) << "}";
	    }
	  genset()->print(out, i.first);
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
    context_t ctx_;
    value_t zero_;
    value_t unit_;
  };

}

#endif // !VCSN_WEIGHTS_POLY_HH
