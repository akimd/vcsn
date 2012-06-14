#ifndef VCSN_CORE_RAT_ABSTRACT_KRATEXPSET_HH
# define VCSN_CORE_RAT_ABSTRACT_KRATEXPSET_HH

# include <string>
# include <list>
# include <memory>  // shared_ptr
# include <sstream>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/kratexpset.hh>

namespace vcsn
{

  class abstract_kratexpset
  {
  public:
    using value_t = rat::exp_t;

    virtual value_t zero() const = 0;
    virtual value_t unit() const = 0;
    virtual value_t atom(const std::string& w) const = 0;
    virtual value_t add(value_t l, value_t r) const = 0;
    /// Explicit product.
    virtual value_t mul(value_t l, value_t r) const = 0;
    /// Implicit product.  If both \a l and \a r are weightless word,
    /// produce a new word that concatenates them.  Otherwise, use \a mul.
    virtual value_t concat(value_t l, value_t r) const = 0;
    virtual value_t star(value_t e) const = 0;
    virtual value_t weight(std::string* w, value_t e) const = 0;
    virtual value_t weight(value_t e, std::string* w) const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
    std::string format(const value_t v) const
    {
      std::stringstream s;
      print(s, v);
      return s.str();
    }
  };

  template <typename Context>
  class concrete_abstract_kratexpset : public abstract_kratexpset
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = typename context_t::kind_t;
    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using super_type = abstract_kratexpset;
    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using atom_value_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    /// When taken as a WeightSet, our (abstract) value type.
    using value_t = rat::exp_t;
    /// Concrete value type.
    using kratexp_t = rat::kratexp<atom_value_t, weight_t>;
    using kvalue_t = typename kratexp_t::kvalue_t;

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    concrete_abstract_kratexpset(const context_t& ctx)
      : super_type()
      , ks_(ctx)
    {}

    // Specialization from abstract_kratexpset.
    virtual value_t zero() const
    {
      return ks_.zero();
    }

    virtual value_t unit() const
    {
      return ks_.unit();
    }

    virtual value_t atom(const word_t& w) const
    {
      return ks_.atom(w);
    }

    virtual value_t add(value_t l, value_t r) const
    {
      auto left = down_pointer_cast<const kratexp_t>(l);
      auto right = down_pointer_cast<const kratexp_t>(r);
      return ks_.add(left, right);
    }

    virtual value_t mul(value_t l, value_t r) const
    {
      auto left = down_pointer_cast<const kratexp_t>(l);
      auto right = down_pointer_cast<const kratexp_t>(r);
      return ks_.mul(left, right);
    }

    /// When concatenating two atoms, possibly make a single one,
    /// or make the product.
    virtual value_t concat(value_t l, value_t r) const
    {
      return ks_.concat(down_pointer_cast<const kratexp_t>(l),
                        down_pointer_cast<const kratexp_t>(r));
    }

    virtual value_t star(value_t e) const
    {
      return ks_.star(down_pointer_cast<const kratexp_t>(e));
    }

    virtual value_t weight(std::string* w, value_t e) const
    {
      auto v = ks_.weightset()->conv(*w);
      delete w;
      return ks_.weight(v, down_pointer_cast<const kratexp_t>(e));
    }

    virtual value_t weight(value_t e, std::string* w) const
    {
      auto v = ks_.weightset()->conv(*w);
      delete w;
      return ks_.weight(down_pointer_cast<const kratexp_t>(e), v);
    }

    virtual std::ostream& print(std::ostream& o, value_t v) const
    {
      return ks_.print(o, down_pointer_cast<const kratexp_t>(v));
    }

  private:
    kratexpset<context_t> ks_;
  };
} // namespace vcsn

#endif // !VCSN_CORE_RAT_ABSTRACT_KRATEXPSET_HH
