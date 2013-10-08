#ifndef VCSN_WEIGHTS_ENTRYSET_HH
# define VCSN_WEIGHTS_ENTRYSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/military-order.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  /// Linear combination of labels: map labels to weights.
  template <class Context>
  class entryset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using label_t = typename labelset_t::label_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<label_t, weight_t, MilitaryOrder<label_t>>;
    /// A pair <label, weight>.
    using monomial_t = typename value_t::value_type;

    entryset() = delete;
    entryset(const entryset&) = default;
    entryset(const context_t& ctx)
      : ctx_{ctx}
    {}

    /// The static name.
    static std::string sname()
    {
      return "entryset<" + context_t::sname() + ">";
    }

    /// The dynamic name.
    std::string vname(bool full = true) const
    {
      return "entryset<" + context().vname(full) + ">";
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

    bool
    equals(const value_t& l, const value_t& r) const ATTRIBUTE_PURE
    {
      return l.size() == r.size()
        && std::equal(l.begin(), l.end(),
                      r.begin());
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
              // The label is not \z.
              label_t label = labelset()->special();
              // Accept an implicit label if there is an explicit weight.
              try
                {
                  label = labelset()->conv(i);
                }
              catch (const std::domain_error&)
                {
                  // We must have at least a weight or a label.
                  if (default_w)
                    throw std::domain_error
                      (std::string{"entryset: conv: invalid value: "}
                       + str_escape(i.peek())
                       + " contains an empty label (did you mean \\e or \\z?)");
                }
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
        throw std::domain_error("entryset: conv: invalid value: " + s
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


  /// Extract the entry between two states of an automaton.
  template <typename Aut>
  typename entryset<typename Aut::context_t>::value_t
  get_entry(const Aut& aut,
            typename Aut::state_t s, typename Aut::state_t d)
  {
    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using entryset_t = entryset<context_t>;
    using entry_t = typename entryset_t::value_t;

    entry_t res;
    for (auto t : aut.outin(s, d))
      // Bypass set_weight(), because we know that the weight is
      // nonzero, and that there is only one weight per letter.
      res[aut.label_of(t)] = aut.weight_of(t);
    return res;
  }

}

#endif // !VCSN_WEIGHTS_ENTRYSET_HH
