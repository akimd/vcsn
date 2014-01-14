#ifndef VCSN_WEIGHTS_POLYNOMIALSET_HH
# define VCSN_WEIGHTS_POLYNOMIALSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/map.hh>
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

  /// Linear combination of labels: map labels to weights.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <class Context>
  class polynomialset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    /// Polynomials over labels.
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

    /// v += p.
    value_t&
    add_weight(value_t& v, const value_t& p) const
    {
      for (const auto& m: p)
        add_weight(v, m);
      return v;
    }

    /// v += m.
    value_t&
    add_weight(value_t& v, const monomial_t& p) const
    {
      return add_weight(v, p.first, p.second);
    }

    /// v += (l, k).
    value_t&
    add_weight(value_t& v, const label_t& l, const weight_t k) const
    {
      auto i = v.find(l);
      if (i == v.end())
        {
          set_weight(v, l, k);
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
      throw std::domain_error(sname() + ": star: invalid value: "
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
                   // FIXME: This is wrong, it should be mul, not concat.
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

    static constexpr bool is_commutative_semiring() { return false; }

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

    static size_t hash(const monomial_t& m)
    {
      size_t res = 0;
      std::hash_combine(res, labelset_t::hash(m.first));
      std::hash_combine(res, weightset_t::hash(m.second));
      return res;
    }

    static size_t hash(const value_t& v)
    {
      return std::hash_value(v);
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
              // FIXME: convert to use conv(std::istream).
              w = ::vcsn::conv(*weightset(), bracketed(i, lbracket, rbracket));
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
              // The label is not \z.

              // Register the current position in the stream, so that
              // we reject inputs such as "a++a" in LAW (where the
              // labelset::conv would accept the empty string between
              // the two "+").
              std::streampos p = i.tellg();
              label_t label = labelset()->special();
              // Accept an implicit label (which can be an error,
              // e.g., for LAL) if there is an explicit weight.
              try
                {
                  label = labelset()->conv(i);
                }
              catch (const std::domain_error&)
                {}
              // We must have at least a weight or a label.
              if (default_w && p == i.tellg())
                throw std::domain_error
                  (sname() + ": conv: invalid value: "
                   + str_escape(i.peek())
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

    /// Print a monomial.
    std::ostream&
    print(std::ostream& out, const monomial_t& m,
          const std::string& format = "text") const
    {
      static bool parens = getenv("VCSN_PARENS");
      if (parens || weightset()->show_one() || !weightset()->is_one(m.second))
        {
          out << (format == "latex" ? "\\langle " : std::string{lbracket});
          weightset()->print(out, m.second, format);
          out << (format == "latex" ? "\\rangle " : std::string{rbracket});
        }
      if (parens)
        out << (format == "latex" ? "\\left(" : "(");
      labelset()->print(out, m.first, format);
      if (parens)
        out << (format == "latex" ? "\\right)" : ")");
      return out;
    }

    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& format = "text",
          const std::string& sep = " + ") const
    {
      if (v.empty())
        out << (format == "latex" ? "\\emptyset" : "\\z");
      else
        {
          bool first = true;
          for (const auto& m: v)
            {
              if (!first)
                out << sep;
              first = false;
              print(out, m, format);
            }
        }
      return out;
    }

    std::string
    format(const value_t& v, const std::string& sep = " + ") const
    {
      std::ostringstream o;
      print(o, v, "text", sep);
      return o.str();
    }

    /// Format a monomial.
    std::string
    format(const monomial_t& m) const
    {
      std::ostringstream o;
      print(o, m, "text");
      return o.str();
    }

  private:
    context_t ctx_;

    /// Left marker for weight in concrete syntax.
    constexpr static char lbracket = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rbracket = '>';
  };

  /// The entry between two states of an automaton.
  template <typename Aut>
  typename polynomialset<typename Aut::context_t>::value_t
  get_entry(const Aut& aut,
            typename Aut::state_t s, typename Aut::state_t d)
  {
    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using polynomialset_t = polynomialset<context_t>;
    using polynomial_t = typename polynomialset_t::value_t;

    polynomial_t res;
    for (auto t : aut.outin(s, d))
      // Bypass set_weight(), because we know that the weight is
      // nonzero, and that there is only one weight per letter.
      res[aut.label_of(t)] = aut.weight_of(t);
    return res;
  }

}

#endif // !VCSN_WEIGHTS_POLYNOMIALSET_HH
