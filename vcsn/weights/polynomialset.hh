#ifndef VCSN_WEIGHTS_POLYNOMIALSET_HH
# define VCSN_WEIGHTS_POLYNOMIALSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <vcsn/dyn/weightset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/military-order.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  template <class Context>
  class polynomialset: public dyn::detail::abstract_weightset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using word_t = typename labelset_t::word_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<word_t, weight_t, MilitaryOrder<word_t>>;

    polynomialset() = delete;
    polynomialset(const polynomialset&) = default;
    polynomialset(const context_t& ctx)
      : ctx_{ctx}
    {
      one_[labelset()->genset()->empty_word()] = weightset()->one();
    }

    /// The static name.
    std::string sname() const
    {
      return "polynomialset<" + context_t::sname() + ">";
    }

    /// The dynamic name.
    std::string vname(bool full = true) const
    {
      return "polynomialset<" + context().vname(full) + ">";
    }

    const context_t& context() const { return ctx_; }
    const labelset_ptr& labelset() const { return ctx_.labelset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

    /// Remove the mononial of \a w in \a v.
    value_t&
    del_weight(value_t& v, const word_t& w) const
    {
      v.erase(w);
      return v;
    }

    /// Set the mononial of \a w in \a v to weight \a k.
    value_t&
    set_weight(value_t& v, const word_t& w, const weight_t k) const
    {
      if (weightset()->is_zero(k))
        del_weight(v, w);
      else
        v[w] = k;
      return v;
    }

    value_t&
    add_weight(value_t& v, const std::pair<word_t, weight_t>& p) const
    {
      return add_weight(v, p.first, p.second);
    }

    value_t&
    add_weight(value_t& v, const word_t& w, const weight_t k) const
    {
      auto i = v.find(w);
      if (i == v.end())
        {
          set_weight(v, w, k);
        }
      else
        {
          // Do not use set_weight() because it would lookup w
          // again and we already have the right iterator.
          auto w2 = weightset()->add(i->second, k);
          if (weightset()->is_zero(w2))
            v.erase(i);
          else
            i->second = w2;
        }
      return v;
    }

    const weight_t
    get_weight(const value_t& v, const word_t& w) const ATTRIBUTE_PURE
    {
      auto i = v.find(w);
      if (i == v.end())
        return weightset()->zero();
      else
        return i->second;
    }

    /// The sum of polynomials \a l and \a r.
    value_t
    add(const value_t& l, const value_t& r) const
    {
      value_t p = l;
      for (auto& i : r)
        add_weight(p, i.first, i.second);
      return p;
    }

    /// The product of polynomials \a l and \a r.
    value_t
    mul(const value_t& l, const value_t& r) const
    {
      value_t p;
      for (auto i: l)
        for (auto j: r)
          add_weight(p,
                    labelset()->concat(i.first, j.first),
                    weightset()->mul(i.second, j.second));
      return p;
    }

    /// The star of polynomial \a v.
    value_t
    star(const value_t& v) const
    {
      // The only starrable polynomialsets are scalars (if they are
      // starrable too).
      auto s = v.size();
      if (s == 0)
        return one();
      if (s == 1)
        {
          auto i = v.find(labelset()->one());
          if (i != v.end())
            {
              value_t p;
              add_weight(p, i->first, weightset()->star(i->second));
              return p;
            }
        }
      throw std::domain_error("polynomialset: star: invalid value: "
                              + format(v));
    }

    bool
    equals(const value_t& l, const value_t& r) const ATTRIBUTE_PURE
    {
      return l.size() == r.size()
        && std::equal(l.begin(), l.end(),
                      r.begin());
    }

    const value_t&
    one() const
    {
      return one_;
    }

    bool
    is_one(const value_t& v) const ATTRIBUTE_PURE
    {
      if (v.size() != 1)
        return false;
      auto i = v.find(labelset()->one());
      if (i == v.end())
        return false;
      return weightset()->is_one(i->second);
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

    static constexpr bool show_one() { return true; }
    static constexpr star_status_t star_status()
    {
      return weightset_t::star_status();
    }

    value_t
    transpose(const value_t& v) const
    {
      value_t res;
      for (const auto& i: v)
        res[labelset()->transpose(i.first)] = weightset()->transpose(i.second);
      return res;
    }

    /// Construct from a string.
    ///
    /// Somewhat more general than a mere reversal of "format",
    /// in particular "a+a" is properly understood as "<2>a" in
    /// char_z.
    ///
    /// \param i    the stream to parse
    /// \param sep  the separator between monomials.
    value_t
    conv(std::istream& i, const char sep = '+') const
    {
      value_t res;
#define SKIP_SPACES()                           \
      while (isspace(i.peek()))                 \
        i.ignore()

      do
        {
          // Possibly a weight in braces.
          SKIP_SPACES();
          weight_t w = weightset()->one();
          bool default_w = true;
          if (i.peek() == lbracket)
            {
              // FIXME: should be converted to use conv(std::istream),
              // but it is not yet available for ratexp.
              w = weightset()->conv(bracketed(i, lbracket, rbracket));
              default_w = false;
            }

          // Possibly, a label.
          SKIP_SPACES();
          // Whether the label is \z.
          bool is_zero = false;
          if (i.peek() == '\\')
            {
              i.ignore();
              if (i.peek() == 'z')
                {
                  is_zero = true;
                  i.ignore();
                }
              else
                i.unget();
            }

          if (!is_zero)
            {
              // Register the current position in the stream.
              std::streampos p = i.tellg();
              // The label is not \z.
              //
              // Do not use labelset()->conv, as for instance in LAL,
              // it will refuse '\e'.  FIXME: we don't check that the
              // letters are valid.
              word_t label = labelset()->genset()->conv(i);
              // We must have at least a weight or a label.
              if (default_w && p == i.tellg())
                throw std::domain_error
                  (std::string{"polynomialset: conv: invalid value: "}
                   + std::string(1, i.peek())
                   + " contains an empty label (did you mean \\e or \\z?)");
              add_weight(res, label, w);
            }

          // sep (e.g., '+'), or stop parsing.
          SKIP_SPACES();
          if (i.peek() == sep)
            i.ignore();
          else
            break;
        }
      while (true);
#undef SKIP_SPACES

      return res;
    }

    /// Construct from a string.
    ///
    /// Somewhat more general than a mere reversal of "format",
    /// in particular "a+a" is properly understood as "<2>a" in
    /// char_z.
    ///
    /// \param s    the string to parse
    /// \param sep  the separator between monomials.
    value_t
    conv(const std::string& s, const char sep = '+') const
    {
      std::istringstream i{s};
      value_t res = conv(i, sep);

      if (i.peek() != -1)
        throw std::domain_error("polynomialset: conv: invalid value: " + s
                                + " unexpected "
                                + std::string{char(i.peek())});
      return res;
    }

    /// Print a monomial.
    std::ostream&
    print(std::ostream& out, const typename value_t::value_type& m) const
    {
      if (weightset()->show_one() || !weightset()->is_one(m.second))
        {
          out << lbracket;
          weightset()->print(out, m.second) << rbracket;
        }
      labelset()->genset()->print(out, m.first);
      return out;
    }

    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& sep = " + ") const
    {
      bool first = true;
      if (v.empty())
        out << "\\z";
      else
        for (const auto& m: v)
          {
            if (!first)
              out << sep;
            first = false;
            print(out, m);
          }

      return out;
    }

    std::string
    format(const value_t& v, const std::string& sep = " + ") const
    {
      std::ostringstream o;
      print(o, v, sep);
      return o.str();
    }

    /// Format a monomial.
    std::string
    format(const typename value_t::value_type& m) const
    {
      std::ostringstream o;
      print(o, m);
      return o.str();
    }

  private:
    context_t ctx_;
    value_t zero_;
    value_t one_;

    /// Left marker for weight in concrete syntax.
    constexpr static char lbracket = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rbracket = '>';
  };

}

#endif // !VCSN_WEIGHTS_POLYNOMIALSET_HH
