#ifndef VCSN_CTX_CONTEXT_HH
# define VCSN_CTX_CONTEXT_HH

# include <cassert>
# include <memory>
# include <string>

# include <vcsn/core/join.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/core/rat/fwd.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{

  template <typename LabelSet, typename WeightSet>
  class context
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
        is_lar = vcsn::is_lar<context>::value,
        is_lat = vcsn::is_lat<context>::value,
        is_law = vcsn::is_law<context>::value
      };
    /// Type of transition labels, and type of RatExp atoms.
    using label_t = typename labelset_t::value_t;
    /// Type of weights.
    using weight_t = typename weightset_t::value_t;
    /// Type of RatExp ratexps objects.
    using node_t = rat::node<label_t, weight_t>;
    using ratexp_t = std::shared_ptr<const node_t>;
    /// Type of RatExp visitor.
    using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

    context(const context& that)
      : context(that.ls_, that.ws_)
    {}

    /// \param ls  the labelset
    /// \param ws  the weightset
    context(const labelset_ptr& ls, const weightset_ptr& ws)
      : ls_{ls}
      , ws_{ws}
    {}

    /// Build a context.
    /// \param ls  the labelset
    /// \param ws  the weightset
    context(const labelset_t& ls, const weightset_t& ws = {})
      : context(std::make_shared<const labelset_t>(ls),
                std::make_shared<const weightset_t>(ws))
    {}

    /// Build a context.
    /// \param gs  the generators
    /// \param ws  the weightset
    //
    // Use SFINAE to avoid requiring labelset_t to define letter_t.
    // labels_are_tuples does not define it for instance.
    //
    // It would be simpler to just use "= {}", but the C++ standard
    // does not support it (and this is properly considered a
    // defect: see http://cplusplus.github.io/LWG/lwg-active.html#2193).
    //
    // Gcc accepts though.
    // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=56922
    // But clang rejects it.
    // http://llvm.org/bugs/show_bug.cgi?id=15724
    template <typename LabelSet2 = labelset_t>
    context(const std::initializer_list<typename LabelSet2::letter_t>& gs,
            const weightset_t& ws = {})
      : context{labelset_t{gs}, ws}
    {}

    /// Build a context whose labelset constructor takes no argument.
    template <typename LabelSet2 = labelset_t>
    context()
      : context{typename std::enable_if<is_lao, labelset_t>::type{},
                weightset_t{}}
    {}

    context& operator=(context&& that)
    {
      if (this != &that)
        {
          std::swap(ls_, that.ls_);
          std::swap(ws_, that.ws_);
        }
      return *this;
    }

    /// The name of this context, built from its parameters.
    /// E.g., "lal_char_b", "law_char_zmin".
    static std::string sname()
    {
      return (labelset_t::sname()
              + "_" + weightset_t::sname());
    }

    std::string vname(bool full = true) const
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

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      labelset()->print_set(o, format);
      if (format == "latex")
        o << "\\rightarrow";
      else
        o << '_';
      return weightset()->print_set(o, format);
    }

    static constexpr bool
    has_one()
    {
      return labelset_t::has_one();
    }

  private:
    labelset_ptr ls_;
    weightset_ptr ws_;
  };

  /*-----------------.
  | join_t, meet_t.  |
  `-----------------*/

  template <typename... ValueSets>
  using meet_t = decltype(meet(std::declval<ValueSets>()...));


  /*-------------------------.
  | Variadic join and meet.  |
  `-------------------------*/

  /// The meet of a single valueset.
  /// Useful for variadic operator on a single argument.
  template <typename ValueSet>
  auto
  meet(const ValueSet& vs)
    -> ValueSet
  {
    return vs;
  }

  template <typename ValueSet1, typename ValueSet2, typename ValueSet3,
            typename... VSs>
  auto
  meet(const ValueSet1& vs1, const ValueSet2& vs2, const ValueSet3& vs3,
       const VSs&... vs)
    -> decltype(meet(meet(vs1, vs2), vs3, vs...))
  {
    return meet(meet(vs1, vs2), vs3, vs...);
  }


  /*-------------------------.
  | join(context, context).  |
  `-------------------------*/

  namespace detail
  {
    /// The join of two contexts.
    template <typename LS1, typename WS1,
              typename LS2, typename WS2>
    struct join_impl<context<LS1, WS1>, context<LS2, WS2>>
    {
      using labelset_t = join_t<LS1, LS2>;
      using weightset_t = join_t<WS1, WS2>;
      using type = context<labelset_t, weightset_t>;

      static type join(const context<LS1, WS1>& ctx1,
                       const context<LS2, WS2>& ctx2)
      {
        return {vcsn::join(*ctx1.labelset(), *ctx2.labelset()),
                vcsn::join(*ctx1.weightset(), *ctx2.weightset())};
      }
    };
  }

  /*-------------------------.
  | meet(context, context).  |
  `-------------------------*/

  /// The meet of two contexts.
  template <typename LhsLabelSet, typename LhsWeightSet,
            typename RhsLabelSet, typename RhsWeightSet>
  auto
  meet(const context<LhsLabelSet, LhsWeightSet>& a,
       const context<RhsLabelSet, RhsWeightSet>& b)
    -> context<meet_t<LhsLabelSet, RhsLabelSet>,
               join_t<LhsWeightSet, RhsWeightSet>>
  {
    auto ls = meet(*a.labelset(), *b.labelset());
    auto ws = join(*a.weightset(), *b.weightset());
    return {ls, ws};
  }

}

#endif // !VCSN_CTX_CONTEXT_HH
