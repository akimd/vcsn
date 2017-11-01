#pragma once

#include <memory>
#include <string>

#include <vcsn/core/join.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/core/rat/fwd.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>

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
        is_lao = vcsn::is_lao<context>::value,
        is_lar = vcsn::is_lar<context>::value,
        is_lat = vcsn::is_lat<context>::value,
        is_law = vcsn::is_law<context>::value,
      };
    /// Type of transition labels, and type of expression atoms.
    using label_t = typename labelset_t::value_t;
    /// Type of weights.
    using weight_t = typename weightset_t::value_t;

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
    context(const labelset_t& ls = {}, const weightset_t& ws = {})
      : context(std::make_shared<const labelset_t>(ls),
                std::make_shared<const weightset_t>(ws))
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
    /// E.g., "lal_char, b", "law_char, zmin".
    static symbol sname()
    {
      static auto res = symbol{"context<"
                               + labelset_t::sname()
                               + ", "
                               + weightset_t::sname()
                               + '>'};
      return res;
    }

    /// Build from the description in \a is.
    static context make(std::istream& is)
    {
      eat(is, "context<");
      auto ls = labelset_t::make(is);
      eat(is, ',');
      while (isspace(is.peek()))
        is.ignore();
      auto ws = weightset_t::make(is);
      eat(is, '>');
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
    print_set(std::ostream& o, format fmt = {}) const
    {
      labelset()->print_set(o, fmt.delimit(false));
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\to";
          break;
        case format::sname:
          o << ", ";
          break;
        case format::text:
          o << " -> ";
          break;
        case format::utf8:
          o << " → ";
          break;
        case format::raw:
          assert(0);
          break;
        }
      weightset()->print_set(o, fmt.delimit(false));
      return o;
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


  /// Shorthand to build a context.
  template <typename LabelSet, typename WeightSet>
  context<LabelSet, WeightSet>
  make_context(const LabelSet& ls, const WeightSet& ws)
  {
    return {ls, ws};
  }

  template <typename LabelSet, typename WeightSet>
  struct is_multitape<context<LabelSet, WeightSet>>
    : is_multitape<LabelSet>
  {};

  template <typename LabelSet, typename WeightSet>
  struct number_of_tapes<context<LabelSet, WeightSet>>
    : number_of_tapes<LabelSet>
  {};


  /*----------.
  | meet_t.   |
  `----------*/

  template <typename... ValueSets>
  using meet_t = decltype(meet(std::declval<ValueSets>()...));

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
        // Don't use braces, otherwise the context constructor that
        // takes a list-initializer for the labelset thinks both
        // values here are letters for the labelset.
        return type(vcsn::join(*ctx1.labelset(), *ctx2.labelset()),
                    vcsn::join(*ctx1.weightset(), *ctx2.weightset()));
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
