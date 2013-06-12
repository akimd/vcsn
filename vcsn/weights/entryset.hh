#ifndef VCSN_WEIGHTS_ENTRYSET_HH
# define VCSN_WEIGHTS_ENTRYSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
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

    using value_t = std::map<label_t, weight_t>;

    entryset() = delete;
    entryset(const entryset&) = default;
    entryset(const context_t& ctx)
      : ctx_{ctx}
    {}

    std::string sname() const
    {
      return "entryset<" + context_t::sname() + ">";
    }

    std::string vname(bool full = true) const
    {
      return "entryset<" + context().vname(full) + ">";
    }

    const context_t& context() const { return ctx_; }
    const labelset_ptr& labelset() const { return ctx_.labelset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

    value_t&
    del_weight(value_t& v, const label_t& w) const
    {
      v.erase(w);
      return v;
    }

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
    get_weight(value_t& v, const label_t& w) const ATTRIBUTE_PURE
    {
      auto i = v.find(w);
      if (i == v.end())
        return weightset()->zero();
      else
        return i->second;
    }

    value_t
    add(const value_t& l, const value_t& r) const
    {
      value_t p = l;
      for (auto& i : r)
        add_weight(p, i.first, i.second);
      return p;
    }

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

    value_t
    transpose(const value_t v) const
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
          weight_t w = weightset()->unit();
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
                       + std::string(1, i.peek())
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

    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& sep = " + ") const
    {
      if (v.empty())
        out << "\\z";
      else
        {
          bool first = true;
          bool show_unit = weightset()->show_unit();

          for (const auto& i: v)
            {
              if (!first)
                out << sep;
              first = false;

              if (show_unit || !weightset()->is_unit(i.second))
                {
                  out << lbracket;
                  weightset()->print(out, i.second) << rbracket;
                }
              labelset()->print(out, i.first);
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

  private:
    context_t ctx_;

    /// Left marker for weight in concrete syntax.
    constexpr static char lbracket = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rbracket = '>';
  };

}

#endif // !VCSN_WEIGHTS_ENTRYSET_HH
