#ifndef VCSN_CTX_CTX_HH
# define VCSN_CTX_CTX_HH

# include <cassert>
# include <memory>
# include <string>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{

  namespace ctx
  {
    template <typename LabelSet, typename WeightSet>
    class context: public dyn::detail::abstract_context
    {
    public:
      using labelset_t = LabelSet;
      using weightset_t = WeightSet;
      using labelset_ptr = std::shared_ptr<const labelset_t>;
      using weightset_ptr = std::shared_ptr<const weightset_t>;

      using kind_t = typename labelset_t::kind_t;
      enum
        {
          is_lal = vcsn::is_lal<context>::value,
          is_lan = vcsn::is_lan<context>::value,
          is_lao = vcsn::is_lao<context>::value,
          is_law = vcsn::is_law<context>::value
        };
      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename labelset_t::label_t;
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

      /// Build a context.
      /// \param ls  the labelset
      /// \param ws  the weight set
      context(const labelset_t& ls, const weightset_t& ws = {})
        : context{std::make_shared<const labelset_t>(ls),
                  std::make_shared<const weightset_t>(ws)}
      {}

      /// Build a context.
      /// \param gs  the generators
      /// \param ws  the weight set
      //
      // It would be simpler to just use "= {}", but the C++ standard
      // does not support it (and this is properly considered a
      // defect: see http://cplusplus.github.io/LWG/lwg-active.html#2193).
      //
      // Gcc accepts though.
      // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=56922
      // But clang rejects it.
      // http://llvm.org/bugs/show_bug.cgi?id=15724
      context(const std::initializer_list<typename labelset_t::letter_t>& gs,
              const weightset_t& ws = {})
        : context{labelset_t{gs}, ws}
      {}

      context()
        : context{labelset_t{}, weightset_t{}}
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

      /// Build from the description in \a is.
      static context make(std::istream& is)
      {
        auto ls = labelset_t::make(is);
        eat(is, '_');
        auto ws = weightset_t::make(is);
        return {ls, ws};
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

    private:
      labelset_ptr ls_;
      weightset_ptr ws_;
    };


    /// The intersection of two contexts.
    template <typename LabelSet, typename WeightSet>
    context<LabelSet, WeightSet>
    intersection(const context<LabelSet, WeightSet>& a,
                 const context<LabelSet, WeightSet>& b)
    {
      auto gs = intersection(*a.labelset(), *b.labelset());
      auto ls = std::make_shared<LabelSet>(gs);
      // FIXME: check compatibility of weightsets.
      return {ls, a.weightset()};
    }

    /// The union of two contexts.
    template <typename LabelSet, typename WeightSet>
    context<LabelSet, WeightSet>
    get_union(const context<LabelSet, WeightSet>& a,
              const context<LabelSet, WeightSet>& b)
    {
      auto gs = get_union(*a.labelset(), *b.labelset());
      auto ls = std::make_shared<LabelSet>(gs);
      // FIXME: check compatibility of weightsets.
      return {ls, a.weightset()};
    }

  }
}

#endif // !VCSN_CTX_CTX_HH
