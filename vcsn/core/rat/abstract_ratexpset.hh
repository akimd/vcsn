#ifndef VCSN_CORE_RAT_ABSTRACT_RATEXPSET_HH
# define VCSN_CORE_RAT_ABSTRACT_RATEXPSET_HH

# include <string>
# include <list>
# include <memory>  // shared_ptr
# include <sstream>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/ratexpset.hh>

namespace vcsn
{

  /// Abstract wrapper around a (typeful) ratexpset.
  ///
  /// Use it when you want to avoid depending on the ratexpset
  /// parameters (e.g., from a parser).  To use it, actually create a
  /// derived class (concrete_abstract_ratexpset) with the given
  /// parameters, but handle as a reference to an abstract_ratexpset.
  class abstract_ratexpset
  {
  public:
    using context_t = dyn::abstract_context;
    using value_t = rat::exp_t;

    virtual value_t zero() const = 0;
    virtual value_t unit() const = 0;
    /// Throws std::domain_error if w is not a valid label_t.
    virtual value_t atom(const std::string& w) const = 0;
    virtual value_t add(value_t l, value_t r) const = 0;
    /// Explicit product.
    virtual value_t mul(value_t l, value_t r) const = 0;
    /// Implicit product.  If both \a l and \a r are weightless words,
    /// produce a new word that concatenates them.  Otherwise, use \a mul.
    virtual value_t concat(value_t l, value_t r) const = 0;
    virtual value_t star(value_t e) const = 0;
    virtual value_t weight(std::string* w, value_t e) const = 0;
    virtual value_t weight(value_t e, std::string* w) const = 0;

    /// Parsing.
    virtual value_t conv(const std::string& s) const = 0;

    virtual const context_t& context() const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
    std::string format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }
  };

  /// Wrapper around a ratexpset.
  template <typename Context>
  class concrete_abstract_ratexpset : public abstract_ratexpset
  {
  public:
    using context_t = Context;
    using super_type = abstract_ratexpset;
    using word_t = typename context_t::labelset_t::word_t;
    using label_t = typename context_t::label_t;
    using weight_t = typename context_t::weight_t;
    using value_t = typename super_type::value_t;
    /// Concrete value type.
    using node_t = typename context_t::node_t;

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    concrete_abstract_ratexpset(const context_t& ctx)
      : super_type()
      , ks_(ctx)
    {}

    /// From weak to strong typing.
    std::shared_ptr<const node_t>
    down(const value_t& v) const
    {
      return down_pointer_cast<const node_t>(v);
    }

    /// From string, to typed weight.
    /// \param w  is deleted
    weight_t
    down(std::string* w) const
    {
      auto res = ks_.weightset()->conv(*w);
      delete w;
      return res;
    }


    /*------------------------------------------.
    | Specializations from abstract_ratexpset.  |
    `------------------------------------------*/

    virtual value_t zero() const override
    {
      return ks_.zero();
    }

    virtual value_t unit() const override
    {
      return ks_.unit();
    }

    virtual value_t atom(const std::string& w) const override
    {
      return atom_<context_t>(w);
    }

    virtual value_t add(value_t l, value_t r) const override
    {
      return ks_.add(down(l), down(r));
    }

    virtual value_t mul(value_t l, value_t r) const override
    {
      return ks_.mul(down(l), down(r));
    }

    /// When concatenating two atoms, possibly make a single one,
    /// or make the product.
    virtual value_t concat(value_t l, value_t r) const override
    {
      return ks_.concat(down(l), down(r));
    }

    virtual value_t star(value_t v) const override
    {
      return ks_.star(down(v));
    }

    virtual value_t weight(std::string* w, value_t v) const override
    {
      return ks_.weight(down(w), down(v));
    }

    virtual value_t weight(value_t v, std::string* w) const override
    {
      return ks_.weight(down(v), down(w));
    }

    /// Parsing.
    virtual value_t conv(const std::string& s) const override
    {
      return ks_.conv(s);
    }

    virtual std::ostream& print(std::ostream& o, value_t v) const override
    {
      return ks_.print(o, down(v));
    }

    virtual const context_t& context() const override
    {
      return ks_.context();
    }

  private:
    // FIXME: These guys should use labelset->conv.
    template <typename Ctx>
    auto
    atom_(const std::string& v) const
      -> if_lau<Ctx, value_t>
    {
      if (!v.empty())
        throw std::domain_error("invalid atom: " + v + ": not empty");
      return ks_.atom({});
    }

    template <typename Ctx>
    auto
    atom_(const std::string& v) const
      -> if_lal<Ctx, value_t>
    {
      if (!ks_.labelset()->is_letter(v))
        throw std::domain_error("invalid atom: " + v + ": not a letter");
      return ks_.atom(v[0]);
    }

    template <typename Ctx>
    auto
    atom_(const std::string& v) const
      -> if_law<Ctx, value_t>
    {
      return ks_.atom(v);
    }

    ratexpset<context_t> ks_;
  };
} // namespace vcsn

#endif // !VCSN_CORE_RAT_ABSTRACT_RATEXPSET_HH
