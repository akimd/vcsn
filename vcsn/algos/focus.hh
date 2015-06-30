#pragma once

#include <vcsn/algos/fwd.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant
#include <vcsn/misc/tuple.hh> // make_index_range

namespace vcsn
{

  /*-----------------.
  | focus_context.   |
  `-----------------*/

  namespace detail
  {
    /// The type of the resulting apparent context when keeping only tape Tape.
    template <size_t Tape, typename Context>
    using focus_context
      = context<typename labelset_t_of<Context>::template valueset_t<Tape>,
                weightset_t_of<Context>>;

    /// The resulting apparent context when keeping only tape Tape.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto make_focus_context(const context<LabelSet, WeightSet>& ctx)
      -> enable_if_t<context<LabelSet, WeightSet>::is_lat,
                     focus_context<Tape, context<LabelSet, WeightSet>>>
    {
      static_assert(Tape < LabelSet::size(),
                    "focus: tape index out of bounds");
      return {ctx.labelset()->template set<Tape>(), *ctx.weightset()};
    }

    /// When the labelset is not a tupleset, require the tape to be 0,
    /// and return it.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto make_focus_context(const context<LabelSet, WeightSet>& ctx)
      -> enable_if_t<!context<LabelSet, WeightSet>::is_lat,
                     context<LabelSet, WeightSet>>
    {
      static_assert(Tape == 0,
                    "focus: cannot extract non-0 tape from a non tupleset"
                    " labelset");
      return ctx;
    }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (focus).
      template <typename Context, typename Tape>
      context
      focus_context(const context& ctx, integral_constant)
      {
        auto& c = ctx->as<Context>();
        return make_context(vcsn::detail::make_focus_context<Tape::value>(c));
      }
    }
  }



  /*-------------------.
  | focus_automaton.   |
  `-------------------*/

  namespace detail
  {
    template <typename A, typename I>
    struct hidden_label_type;

    template <typename Aut, std::size_t... I>
    struct hidden_label_type<Aut, index_sequence<I...>>
    {
      using ls_t = labelset_t_of<Aut>;
      using type = tupleset<typename ls_t::template valueset_t<I>...>;
    };


    /// Read-write on an automaton, that hides all tapes but one.
    template <std::size_t Tape, typename Aut>
    class focus_automaton_impl
      : public automaton_decorator<Aut,
                                   focus_context<Tape, context_t_of<Aut>>>
    {
    public:
      /// The type of the wrapped automaton.
      using automaton_t = Aut;

      static_assert(context_t_of<Aut>::is_lat,
                    "focus: requires labels_are_tuples");
      static_assert(Tape < labelset_t_of<Aut>::size(),
                    "focus: invalid tape number");

      /// This automaton's state and transition types are those of the
      /// wrapped automaton.
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      /// Underlying automaton context.
      using full_context_t = context_t_of<automaton_t>;
      /// Underlying automaton labelset.
      using full_labelset_t = typename full_context_t::labelset_t;
      /// Underlying automaton label.
      using full_label_t = typename full_labelset_t::value_t;

      /// Exposed context.
      using context_t = focus_context<Tape, full_context_t>;

      /// Exposed labelset.
      using labelset_t = typename context_t::labelset_t;
      using labelset_ptr = typename context_t::labelset_ptr;
      using label_t = typename labelset_t::value_t;

      /// Exposed weightset.
      using weightset_t = typename context_t::weightset_t;
      using weightset_ptr = typename context_t::weightset_ptr;
      using weight_t = typename weightset_t::value_t;

      /// The type of automata to produce this kind of automata.  For
      /// instance, insplitting on a focus_automaton<const
      /// mutable_automaton<Ctx>> should yield a
      /// focus_automaton<mutable_automaton<Ctx>>, without the "inner"
      /// const.
      ///
      /// Note that we discard the demanded Context.  This will
      /// probably raise some problems at some point, but the reason
      /// is that when we "make_fresh_automaton" from a
      /// focus_automaton, we must build a focus_automaton, which
      /// requires the full context!  Typical example is focusing on
      /// tape 0 for a "LAL x LAL" is a "LAL" automaton, but making
      /// this is focus_automaton<0, LAL> is obviously wrong.
      ///
      /// Unfortunately the compilers tend to issue error messages
      /// that make this hard to understand, even though we have some
      /// static assertions.
      template <typename = void>
      using fresh_automaton_t
        = focus_automaton<Tape,
                          fresh_automaton_t_of<automaton_t, full_context_t>>;

      /// Indices of the remaining tapes.
      using hidden_indices_t
        = concat_sequence
          <make_index_range_t<0, Tape>,
           make_index_range_t<Tape + 1,
                              std::tuple_size<full_label_t>::value - Tape - 1>>;

      // All tapes except the exposed one.
      using res_labelset_t
        = typename hidden_label_type<Aut, hidden_indices_t>::type;
      using res_label_t = typename res_labelset_t::value_t;

      using super_t = automaton_decorator<automaton_t, context_t>;

    public:

      focus_automaton_impl(const full_context_t& ctx)
        : focus_automaton_impl(make_shared_ptr<automaton_t>(ctx))
      {}

      focus_automaton_impl(const automaton_t& aut)
        : super_t(aut)
      {}

      static symbol sname()
      {
        static symbol res(("focus_automaton<" + std::to_string(Tape) + ", "
                + automaton_t::element_type::sname() + '>'));
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "focus_automaton<" << std::to_string(Tape) << ", ";
        aut_->print_set(o, format);
        return o << '>';
      }

      full_context_t full_context() const
      {
        return aut_->context();
      }

      context_t context() const
      {
        return context_;
      }

      res_label_t
      hidden_label_of(transition_t t) const
      {
        return hidden_label_of_(t, hidden_indices);
      }

      res_label_t
      hidden_one() const
      {
        return hidden_one_<full_labelset_t>(hidden_indices);
      }

      res_labelset_t
      res_labelset() const
      {
        return res_labelset_(hidden_indices);
      }

      /// Apparent labelset.
      std::shared_ptr<labelset_t>
      labelset() const
      {
        return std::make_shared<labelset_t>(aut_->labelset()->template set<Tape>());
      }

    private:
      using super_t::aut_;

      hidden_indices_t hidden_indices{};

      static label_t hide_(full_label_t l)
      {
        return std::get<Tape>(l);
      }

      template <std::size_t... I>
      res_label_t hidden_label_of_(transition_t t, index_sequence<I...>) const
      {
        full_label_t l = aut_->label_of(t);
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      vcsn::enable_if_t<L::has_one(), res_label_t>
      hidden_one_(index_sequence<I...>) const
      {
        full_label_t l = aut_->labelset()->one();
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      vcsn::enable_if_t<!L::has_one(), res_label_t>
      hidden_one_(index_sequence<I...>) const
      {
        raise("Should not get here");
      }

      template <std::size_t... I>
      res_labelset_t res_labelset_(index_sequence<I...>) const
      {
        return res_labelset_t{std::get<I>(aut_->labelset()->sets())...};
      }

    public:

      /*----------------------------.
      | const methods that change.  |
      `----------------------------*/

      /// Apparent label of a transition.
      auto label_of(transition_t t) const
        -> label_t
      {
        return hide_(aut_->label_of(t));
      }

      // FIXME: http://llvm.org/bugs/show_bug.cgi?id=20175.
      // using super_t::out;
      auto
      out(state_t s) const
        -> decltype(aut_->out(s))
      {
        return aut_->out(s);
      }

      /// Indexes of all transitions leaving state \a s on label \a l.
      /// Invalidated by del_transition() and del_state().
      ///
      /// FIXME: costly structure.
      std::vector<transition_t>
      out(state_t s, label_t l) const
      {
        std::vector<transition_t> res;
        for (auto t: aut_->all_out(s))
          if (labelset()->equal(label_of(t), l))
            res.emplace_back(t);
        return res;
      }

      /// Indexes of all transitions leaving state \a s on label \a l.
      /// Invalidated by del_transition() and del_state().
      ///
      /// FIXME: costly structure.
      using super_t::in;
      std::vector<transition_t>
      in(state_t s, label_t l) const
      {
        std::vector<transition_t> res;
        for (auto t: aut_->all_in(s))
          if (this->labelset()->equal(label_of(t), l))
            res.emplace_back(t);
        return res;
      }

      // FIXME: Having support for predicates in
      // mutable_automaton::get_transition would help.
      transition_t
      get_transition(state_t src, state_t dst, label_t l) const
      {
        for (auto t: out(src, l))
          if (aut_->dst_of(t) == dst)
            return t;
        return aut_->null_transition();
      }

      /// Apparent label of a transition.
      bool
      has_transition(state_t src, state_t dst, label_t l) const
      {
        return get_transition(src, dst, l) != aut_->null_transition();
      }

      using super_t::del_transition;
      /// Apparent label of a transition.
      void
      del_transition(state_t src, state_t dst, label_t l)
      {
        auto t = get_transition(src, dst, l);
        if (t != aut_->null_transition())
          aut_->del_transition(t);
      }

      /// Copy the full wrapped transition.
      template <typename A>
      transition_t
      new_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          typename A::element_type::transition_t t,
                          bool transpose = false)
      {
        return aut_->new_transition_copy(src, dst,
                                         aut->strip(), t, transpose);
      }

      /// Copy the full wrapped transition.
      template <typename A>
      weight_t
      add_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          typename A::element_type::transition_t t,
                          bool transpose = false)
      {
        return aut_->add_transition_copy(src, dst,
                                         aut->strip(), t, transpose);
      }

#define DEFINE(Name, Sig)                       \
      auto Name Sig                             \
      {                                         \
        raise("focus: cannot provide " #Name);  \
      }

      DEFINE(add_transition,
             (state_t, state_t, label_t, weight_t) -> weight_t);
      DEFINE(add_transition,
             (state_t, state_t, label_t) -> weight_t);
      DEFINE(new_transition,
             (state_t, state_t, label_t, weight_t) -> transition_t);
      DEFINE(new_transition,
             (state_t, state_t, label_t) -> transition_t);
      DEFINE(set_transition,
             (state_t, state_t, label_t, weight_t) -> transition_t);
#undef DEFINE

    private:
      /// Our apparent context.  This is not a luxury to cache it:
      /// benches show that in some cases, intensive (and admittedly
      /// wrong: they should have been cached on the caller side)
      /// calls to context() ruins the performances.
      context_t context_ = make_focus_context<Tape>(full_context());
    };
  }

  template <unsigned Tape, typename Aut>
  inline
  focus_automaton<Tape, Aut>
  focus(Aut aut)
  {
    return std::make_shared<detail::focus_automaton_impl<Tape, Aut>>(aut);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Tape>
      automaton
      focus(const automaton& aut, integral_constant)
      {
        auto& a = aut->as<Aut>();
        return make_automaton(vcsn::focus<Tape::value>(a));
      }
    }
  }
}
