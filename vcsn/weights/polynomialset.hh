#ifndef VCSN_WEIGHTS_POLYNOMIALSET_HH
# define VCSN_WEIGHTS_POLYNOMIALSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <vcsn/dyn/polynomialset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  template <typename LabelSet>
  struct labelset_less_than
  {
    using label_t = typename LabelSet::value_t;
    bool operator()(const label_t& lhs, const label_t& rhs) const
    {
      return LabelSet::less_than(lhs, rhs);
    }
  };

  /// Linear combination of words: map words to weights.
  template <class Context>
  class polynomialset: public dyn::detail::abstract_polynomialset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    /// Polynomials over words.
    using label_t = typename labelset_t::value_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<label_t, weight_t, labelset_less_than<labelset_t>>;
    /// A pair <label, weight>.
    using monomial_t = typename value_t::value_type;

    polynomialset() = delete;
    polynomialset(const polynomialset&) = default;
    polynomialset(const context_t& ctx)
      : ctx_{ctx}
    {}

    /// The static name.
    static std::string sname()
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

    /// Remove the monomial of \a w in \a v.
    value_t&
    del_weight(value_t& v, const label_t& w) const
    {
      v.erase(w);
      return v;
    }

    /// Set the monomial of \a w in \a v to weight \a k.
    value_t&
    set_weight(value_t& v, const label_t& w, const weight_t k) const
    {
      if (weightset()->is_zero(k))
        del_weight(v, w);
      else
        v[w] = k;
      return v;
    }

    value_t&
    add_weight(value_t& v, const monomial_t& p) const
    {
      return add_weight(v, p.first, p.second);
    }

    value_t&
    add_weight(value_t& v, const label_t& w, const weight_t k) const
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
    get_weight(const value_t& v, const label_t& w) const ATTRIBUTE_PURE
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
      value_t res = l;
      for (auto& i : r)
        add_weight(res, i.first, i.second);
      return res;
    }

    /// The product of polynomials \a l and \a r.
    value_t
    mul(const value_t& l, const value_t& r) const
    {
      value_t res;
      for (auto i: l)
        for (auto j: r)
          add_weight(res,
                     labelset()->concat(i.first, j.first),
                     weightset()->mul(i.second, j.second));
      return res;
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
              value_t res;
              add_weight(res, i->first, weightset()->star(i->second));
              return res;
            }
        }
      throw std::domain_error("polynomialset: star: invalid value: "
                              + format(v));
    }

    /// Left exterior product.
    value_t
    lmul(const weight_t& w, const value_t& v) const
    {
      value_t res;
      if (!weightset()->is_zero(w))
        // FIXME: What if there are divisors of 0?
        for (const auto& m: v)
          add_weight(res, m.first, weightset()->mul(w, m.second));
      return res;
    }

    /// Left product by a label.
    value_t
    lmul(const label_t& lhs, const value_t& v) const
    {
      value_t res;
      for (auto i: v)
        add_weight(res,
                   labelset()->concat(lhs, i.first),
                   i.second);
      return res;
    }

    /// Right exterior product.
    value_t
    rmul(const value_t& v, const weight_t& w) const
    {
      value_t res;
      if (!weightset()->is_zero(w))
        for (const auto& m: v)
          add_weight(res, m.first, weightset()->mul(m.second, w));
      return res;
    }

    /// Right product.
    value_t
    rmul(const value_t& v, const label_t& rhs) const
    {
      value_t res;
      for (auto i: v)
        add_weight(res,
                   labelset()->concat(i.first, rhs),
                   i.second);
      return res;
    }

    bool
    equals(const value_t& l, const value_t& r) const ATTRIBUTE_PURE
    {
      return l.size() == r.size()
        // FIXME: this is wrong, it uses operator== instead of equals().
        && std::equal(l.begin(), l.end(),
                      r.begin());
    }

    const value_t&
    one() const
    {
      static value_t one_{{labelset()->one(), weightset()->one()}};
      return one_;
    }

    const monomial_t&
    monomial_one() const
    {
      // Singleton.
      static monomial_t res{labelset()->one(), weightset()->one()};
      return res;
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
      static value_t zero_;
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
              label_t label = labelset()->genset()->conv(i);
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
    print(std::ostream& out, const monomial_t& m) const
    {
      if (weightset()->show_one() || !weightset()->is_one(m.second))
        {
          out << lbracket;
          weightset()->print(out, m.second) << rbracket;
        }
      labelset()->print(out, m.first);
      return out;
    }

    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& sep = " + ") const
    {
      if (v.empty())
        out << "\\z";
      else
        {
          bool first = true;
          for (const auto& m: v)
            {
              if (!first)
                out << sep;
              first = false;
              print(out, m);
            }
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
    format(const monomial_t& m) const
    {
      std::ostringstream o;
      print(o, m);
      return o.str();
    }

  private:
    context_t ctx_;

    /// Left marker for weight in concrete syntax.
    constexpr static char lbracket = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rbracket = '>';
  };

}

#endif // !VCSN_WEIGHTS_POLYNOMIALSET_HH
