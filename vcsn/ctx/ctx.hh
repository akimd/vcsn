#ifndef VCSN_CTX_CTX_HH
# define VCSN_CTX_CTX_HH

# include <cassert>
# include <memory>
# include <string>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/ctx/abstract_context.hh>

namespace vcsn
{

  namespace ctx
  {
    template <typename LabelSet, typename WeightSet>
    class context: public dyn::abstract_context
    {
    public:
      using labelset_t = LabelSet;
      using weightset_t = WeightSet;
      using labelset_ptr = std::shared_ptr<const labelset_t>;
      using weightset_ptr = std::shared_ptr<const weightset_t>;

      using kind_t = typename labelset_t::kind_t;
      enum
        {
          is_lau = std::is_same<kind_t, labels_are_unit>::value,
          is_lal = std::is_same<kind_t, labels_are_letters>::value,
          is_law = std::is_same<kind_t, labels_are_words>::value,
        };

      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename labelset_t::label_t;
      using word_t = typename labelset_t::word_t;
      /// Type of weights.
      using weight_t = typename weightset_t::value_t;
      /// Type of RatExp ratexps objects.
      using node_t = rat::node<label_t, weight_t>;
      using ratexp_t = std::shared_ptr<const node_t>;
      using ratexpset_t = typename vcsn::ratexpset<context>;
      /// Type of RatExp visitor.
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      context(const context& that)
        : ls_{that.ls_}
        , ws_{that.ws_}
      {}

      /// \param ls  the labelset
      /// \param ws  the weight set
      context(const labelset_ptr& ls, const weightset_ptr& ws)
        : ls_{ls}
        , ws_{ws}
      {}

      /// \param ls  the labelset
      /// \param ws  the weight set
      context(const labelset_t& ls, const weightset_t& ws = {})
        : context{std::make_shared<const labelset_t>(ls),
                  std::make_shared<const weightset_t>(ws)}
      {}

      /// \param gs  the generators
      /// \param ws  the weight set
      context(const typename labelset_t::letters_t& gs = {},
              const weightset_t& ws = {})
        : context{labelset_t{gs}, ws}
      {}

      /// The name of this context, built from its parameters.
      /// E.g., "lal_char_b", "law_char_zmin".
      static std::string sname()
      {
        return (labelset_t::sname()
                + "_" + weightset_t::sname());
      }

      virtual std::string vname(bool full = true) const override final
      {
        return (labelset()->vname(full)
                + "_" + weightset()->vname(full));
      }

      const labelset_ptr& labelset() const
      {
        return ls_;
      }
      const weightset_ptr& weightset() const
      {
        return ws_;
      }

      ratexpset_t
      make_ratexpset() const
      {
        return *this;
      }

      std::string format(const ratexp_t& e) const
      {
        ratexpset_t ks{*this};
        return ks.format(e);
      }

      ratexp_t
      downcast(const rat::exp_t& e) const
      {
        ratexp_t res = std::dynamic_pointer_cast<const node_t>(e);
        assert(res);
        return res;
      }

    private:
      labelset_ptr ls_;
      weightset_ptr ws_;
    };

  }

  // Provide "overloading" on parameter types, for instance to require
  // letter_t when labels_are_letters, and word_t when
  // labels_are_words.  See ratexpset::atom and ratexpset::atom_ for
  // an example.
  //
  // It is very tempting to turns these guys into members of
  // ctx::context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  // one must write "(typename Cxx::template if_lal<letter_t> v)".
  template <typename Ctx, typename R>
  using if_lau = typename std::enable_if<Ctx::is_lau, R>::type;

  template <typename Ctx, typename R>
  using if_lal = typename std::enable_if<Ctx::is_lal, R>::type;

  template <typename Ctx, typename R>
  using if_law = typename std::enable_if<Ctx::is_law, R>::type;
}

#endif // !VCSN_CTX_CTX_HH
