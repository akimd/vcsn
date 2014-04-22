#ifndef VCSN_WEIGHTSETS_POLYNOMIALSET_HH
# define VCSN_WEIGHTSETS_POLYNOMIALSET_HH

# include <algorithm>
# include <iostream>
# include <sstream>
# include <type_traits>
# include <vector>

# include <vcsn/ctx/context.hh> // We need context to define join.
# include <vcsn/weightset/fwd.hh>

# include <vcsn/ctx/traits.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/map.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  // http://llvm.org/bugs/show_bug.cgi?id=18571
# if defined __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunused-value"
# endif
  template <typename LabelSet>
  auto label_is_zero(const LabelSet& ls, const typename LabelSet::value_t* l)
    -> decltype(ls.is_zero(l), bool())
  {
    return ls.is_zero(*l);
  }

# if defined __clang__
# pragma clang diagnostic pop
# endif

  template <typename LabelSet>
  bool label_is_zero(const LabelSet&, ...)
  ATTRIBUTE_CONST;

  template <typename LabelSet>
  bool label_is_zero(const LabelSet&, ...)
  {
    return false;
  }

  /// Linear combination of labels: map labels to weights.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <class Context>
  class polynomialset
  {
  public:
    using self_type = polynomialset<Context>;
    using context_t = Context;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using polynomialset_t = polynomialset<context_t>;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    /// Polynomials over labels.
    using label_t = typename labelset_t::value_t;
    using weight_t = weight_t_of<context_t>;

    using value_t = std::map<label_t, weight_t, vcsn::less<labelset_t>>;
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

    // FIXME: rename at least this version.
    /// v += (l, k).
    value_t&
    add_weight(value_t& v, const label_t& l, const weight_t k) const
    {
      if (!label_is_zero(*labelset(), &l))
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

    /// The conjunction of polynomials \a l and \a r.
    /// Not valid for all the labelsets.
    value_t
    conjunction(const value_t& l, const value_t& r) const
    {
      value_t res;
      for (auto i: l)
        for (auto j: r)
          add_weight(res,
                     labelset()->conjunction(i.first, j.first),
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
      else if (s == 1)
        {
          auto i = v.find(labelset()->one());
          if (i != v.end())
            {
              value_t res;
              add_weight(res, i->first, weightset()->star(i->second));
              return res;
            }
        }
      raise(sname(), ": star: invalid value: ", vcsn::format(*this, v));
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

    static value_t
    rdiv(const value_t&, const value_t&)
    {
      throw std::runtime_error("not implemented for polynomials");
    }

    static value_t
    ldiv(const value_t& l, const value_t& r)
    {
      return rdiv(r, l);
    }

    static bool
    equals(const value_t& l, const value_t& r) ATTRIBUTE_PURE
    {
      return l.size() == r.size()
        // FIXME: this is wrong, it uses operator== instead of equals().
        && std::equal(l.begin(), l.end(),
                      r.begin());
    }

    const value_t&
    one() const
    {
      static value_t one_{monomial_one()};
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

    /// Conversion from (this and) other weightsets.
    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

  private:
    value_t
    conv_from_this_weightset(const typename weightset_t::value_t& v) const
    {
      monomial_t m{labelset()->one(),
                   weightset()->mul(v, weightset()->one())};
      return value_t{m};
    }

  public:
    /// FIXME: use enable_if to prevent this from being instantiated
    /// when WS is a polynomialset.  Then use this same technique for
    /// ratexps.
    template <typename WS>
    value_t
    conv(const WS& ws, const typename WS::value_t& v) const
    {
      return conv_from_this_weightset(weightset()->conv(ws, v));
    }

    /// Convert from another polynomialset to type_t.
    template <typename C>
    value_t
    conv(const polynomialset<C>& sps,
         const typename polynomialset<C>::value_t& v) const
    {
      value_t res;
      typename C::labelset_t sls = * sps.labelset();
      typename C::weightset_t sws = * sps.weightset();
      labelset_t tls = * labelset();
      weightset_t tws = * weightset();
      for (const auto& m: v)
        // FIXME: rename this version of add_weight.
        add_weight(res, tls.conv(sls, m.first), tws.conv(sws, m.second));
      return res;
    }


    static bool monomial_less_than(const monomial_t& lhs,
                                   const monomial_t& rhs)
    {
      if (labelset_t::less_than(lhs.first, rhs.first))
        return true;
      else if (labelset_t::less_than(rhs.first, lhs.first))
        return false;
      else
        return weightset_t::less_than(lhs.second, rhs.second);
    }

    static bool less_than(const value_t& lhs,
                          const value_t& rhs)
    {
      return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                          rhs.begin(), rhs.end(),
                                          monomial_less_than);
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
      size_t res = 0;
      for (const auto& m: v)
        {
          std::hash_combine(res, labelset_t::hash(m.first));
          std::hash_combine(res, weightset_t::hash(m.second));
        }
      return res;
    }

    /// Build from the description in \a is.
    static self_type make(std::istream& is)
    {
      // name is, for instance, "polynomialset<lal_char(abcd)_z>".
      eat(is, "polynomialset<");
      auto ctx = Context::make(is);
      eat(is, '>');
      return {ctx};
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      if (format == "latex")
        {
          o << "\\mathsf{Poly}[";
          labelset()->print_set(o, format);
          o << " \\to ";
          weightset()->print_set(o, format);
          o << "]";
        }
      else
        {
          o << "polynomialset<";
          labelset()->print_set(o, format);
          o << "_";
          weightset()->print_set(o, format);
          o << ">";
        }
      return o;
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
          if (i.peek() == langle)
            {
              // FIXME: convert to use conv(std::istream).
              w = ::vcsn::conv(*weightset(), bracketed(i, langle, rangle));
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

              // Handle ranges
              if (i.peek() == '[')
                for (auto c : labelset()->convs(i))
                  add_weight(res, c, w);
              else
                {

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
                    raise(sname(), ": conv: invalid value: ",
                          str_escape(i.peek()),
                          " contains an empty label"
                          " (did you mean \\e or \\z?)");
                  add_weight(res, label, w);
                }
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
      print_(out, m.second, format);
      if (parens)
        out << (format == "latex" ? "\\left(" : "(");
      labelset()->print(out, m.first, format);
      if (parens)
        out << (format == "latex" ? "\\right)" : ")");
      return out;
    }

    /// Print a value (a polynomial).
    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& format = "text",
          const std::string& sep = " + ") const
    {
      bool latex = format == "latex";
      if (v.empty())
        out << (latex ? "\\emptyset" : "\\z");
      else
        print_<context_t>(out, v, format,
                          latex && sep == " + " ? " \\oplus " : sep);
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
    /// Print a weight.
    std::ostream&
    print_(std::ostream& out, const weight_t& w,
           const std::string& format = "text") const
    {
      static bool parens = getenv("VCSN_PARENS");
      if (parens || weightset()->show_one() || !weightset()->is_one(w))
        {
          out << (format == "latex" ? "\\langle " : std::string{langle});
          weightset()->print(out, w, format);
          out << (format == "latex" ? "\\rangle " : std::string{rangle});
        }
      return out;
    }


    /// Print a polynomial value without ranges.
    std::ostream&
    print_without_ranges_(std::ostream& out, const value_t& v,
                          const std::string& format = "text",
                          const std::string& sep = " + ") const
    {
      bool first = true;
      for (const auto& m: v)
        {
          if (!first)
            out << sep;
          first = false;
          print(out, m, format);
        }
      return out;
    }

    /// Print a polynomial value with ranges.
    std::ostream&
    print_with_ranges_(std::ostream& out, const value_t& v,
                       const std::string& format = "text",
                       const std::string& sep = " + ") const
    {
      if (sep == " + " || v.size() <= 2)
        return print_without_ranges_(out, v, format, sep);

      // No ranges if the weights aren't all the same.
      std::vector<label_t> letters;
      weight_t first_w = weightset()->zero();
      for (const auto& m: v)
        {
          if (weightset()->is_zero(first_w))
            first_w = m.second;
          else if (!weightset()->equals(m.second, first_w))
            return print_without_ranges_(out, v, format, sep);
          letters.push_back(m.first);
        }

      // Print with ranges.  First, the constant-term.
      if (labelset()->is_one(std::begin(v)->first))
        {
          print(out, *std::begin(v), format);
          if (1 < v.size())
            out << sep;
        }

      // The weight.
      print_(out, first_w, format);

      // Print the character class.  letters are sorted, since
      // polynomials are shortlex-sorted on the labels.
      out << '[';
      const auto& alphabet = labelset()->genset();
      for (auto it = std::begin(letters), letters_end = std::end(letters);
           it != letters_end; ++it)
        {
          auto end = std::mismatch(it, letters_end, alphabet.find(*it)).first;
          labelset()->print(out, *it, format);
          // No ranges for less than two letters.
          auto width = std::distance(it, end);
          if (2 <= width)
            {
              it += width - 1;
              out << '-';
              labelset()->print(out, *it, format);
            }
        }
      out << ']';

      return out;
    }

    /// Print a non-null value for neither LAL nor LAN.
    template <typename context>
    typename std::enable_if<!(context::is_lal || context::is_lan),
                            std::ostream&>::type
    print_(std::ostream& out, const value_t& v,
           const std::string& format = "text",
           const std::string& sep = " + ") const
    {
      return print_without_ranges_(out, v, format, sep);
    }

    /// Print a non-null value for LAL or LAN.
    template <typename Ctx>
    typename std::enable_if<Ctx::is_lal || Ctx::is_lan,
                            std::ostream&>::type
    print_(std::ostream& out, const value_t& v,
           const std::string& format = "text",
           const std::string& sep = " + ") const
    {
      return print_with_ranges_(out, v, format, sep);
    }


  private:
    context_t ctx_;

    /// Left marker for weight in concrete syntax.
    constexpr static char langle = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rangle = '>';
  };

  /// The entry between two states of an automaton.
  template <typename Aut>
  typename polynomialset<context_t_of<Aut>>::value_t
  get_entry(const Aut& aut,
            state_t_of<Aut> s, state_t_of<Aut> d)
  {
    using automaton_t = Aut;
    using context_t = context_t_of<automaton_t>;
    using polynomialset_t = polynomialset<context_t>;
    using polynomial_t = typename polynomialset_t::value_t;

    polynomial_t res;
    for (auto t : aut.outin(s, d))
      // Bypass set_weight(), because we know that the weight is
      // nonzero, and that there is only one weight per letter.
      res[aut.label_of(t)] = aut.weight_of(t);
    return res;
  }

  // FIXME: this works perfectly well, but I'd like a two-parameter version.
  template <typename PLS1, typename PWS1,
            typename PLS2, typename PWS2>
  inline
  auto
  join(const polynomialset<context<PLS1, PWS1>>& p1,
       const polynomialset<context<PLS2, PWS2>>& p2)
    -> polynomialset<context<join_t<PLS1, PLS2>,
                             join_t<PWS1, PWS2>>>
  {
    return {join(p1.context(), p2.context())};
  }

  template <typename WS1,
            typename PLS2, typename PWS2>
  inline
  auto
  join(const WS1& w1,
       const polynomialset<context<PLS2, PWS2>>& p2)
    -> polynomialset<context<PLS2, join_t<WS1, PWS2>>>
  {
    using ctx_t = context<PLS2, join_t<WS1, PWS2>>;
    return ctx_t{* p2.labelset(), join(w1, * p2.weightset())};
  }

  template <typename PLS1, typename PWS1,
            typename WS2>
  inline
  auto
  join(const polynomialset<context<PLS1, PWS1>>& p1,
       const WS2& w2)
    -> polynomialset<context<PLS1, join_t<PWS1, WS2>>>
  {
    return join(w2, p1);
  }

}

#endif // !VCSN_WEIGHTSETS_POLYNOMIALSET_HH
