#ifndef VCSN_WEIGHTS_POLYNOMIALSET_HH
# define VCSN_WEIGHTS_POLYNOMIALSET_HH

# include <iostream>
# include <map>
# include <sstream>

# include <vcsn/weights/fwd.hh>
# include <misc/star_status>

namespace vcsn
{
  template <class Context>
  struct polynomialset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using word_t = typename labelset_t::word_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<word_t, weight_t>;

    polynomialset() = delete;
    polynomialset(const polynomialset&) = default;
    polynomialset(const context_t& ctx)
      : ctx_{ctx}
    {
      unit_[labelset()->identity()] = weightset()->unit();
    }

    std::string sname() const
    {
      return "polynomialset<" + context_t::sname() + ">";
    }

    std::string vname(bool full = true) const
    {
      return "polynomialset<" + context().vname(full) + ">";
    }


    const context_t& context() const { return ctx_; }
    const labelset_ptr& labelset() const { return ctx_.labelset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

    value_t&
    del_weight(value_t& v, const word_t& w) const
    {
      v.erase(w);
      return v;
    }

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
    get_weight(value_t& v, const word_t& w) const
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
    star(const value_t& v) const
    {
      // The only starable polynomialsets are scalars (if they are
      // starable too).
      auto s = v.size();
      if (s == 0)
        return unit();
      if (s == 1)
        {
          auto i = v.find(labelset()->identity());
          if (i != v.end())
            {
              value_t p;
              add_weight(p, i->first, weightset()->star(i->second));
              return p;
            }
        }
      throw std::domain_error("polynomialset: star: invalid value: " + format(v));
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
      auto i = v.find(labelset()->identity());
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
    static constexpr star_status_t star_status()
    {
      return weightset_t::star_status();
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
    /// in particular "a+a" is properly understood as "{2}a" in
    /// char_z.
    ///
    /// \param s    the string to parse
    /// \param sep  the separator between monomials.
    value_t
    conv(const std::string& s, const char sep = '+') const
    {
      value_t res;
      std::istringstream i{s};
      std::ostringstream o;

#define SKIP_SPACES()                           \
      while (isspace(i.peek()))                 \
        i.ignore()

      do
        {
          // Possibly a weight in braces.
          SKIP_SPACES();
          weight_t w = weightset()->unit();
          bool default_w = true;
          if (i.peek() == '{')
            {
              i.ignore();
              size_t level = 1;
              o.clear();
              o.str("");
              while (true)
                {
                  if (i.peek() == '{')
                    ++level;
                  else if (i.peek() == '}'
                           && !--level)
                    {
                      i.ignore();
                      break;
                    }
                  o << char(i.get());
                }
              w = weightset()->conv(o.str());
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
              word_t label = labelset()->conv(i);
              // We must have at least a weight or a label.
              if (default_w && p == i.tellg())
                throw std::domain_error("invalid polynomialset: " + s
                                        + " contains an empty label "
                                        "(did you mean \\e or \\z?)");
              add_weight(res, label, w);
            }

          // EOF, or sep (e.g., '+').
          SKIP_SPACES();
          if (i.peek() == -1)
            break;
          else if (i.peek() == sep)
            i.ignore();
          else
            {
              throw std::domain_error("invalid polynomialset: " + s
                                      + " unexpected "
                                      + std::string{char(i.peek())});
              i.ignore();
            }
        }
      while (true);
#undef SKIP_SPACES

      return res;
    }

    std::ostream&
    print(std::ostream& out, const value_t& v,
          const std::string& sep = " + ") const
    {
      bool first = true;
      bool show_unit = weightset()->show_unit();

      if (v.empty())
        out << "\\z";
      else
        for (const auto& i: v)
          {
            if (!first)
              out << sep;
            first = false;

            if (show_unit || !weightset()->is_unit(i.second))
              {
                out << "{";
                weightset()->print(out, i.second) << "}";
              }
            // FIXME: Should be invisible.
            if (!context_t::is_lau)
              labelset()->print(out, i.first);
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
    value_t zero_;
    value_t unit_;
  };

}

#endif // !VCSN_WEIGHTS_POLYNOMIALSET_HH
