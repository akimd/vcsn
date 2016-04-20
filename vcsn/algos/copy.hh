#pragma once

#include <unordered_map>

#include <vcsn/algos/fwd.hh> // focus_automaton.
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/fwd.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/copy.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/unordered_set.hh>

namespace vcsn
{

  namespace detail
  {
    /// When we copy a focus automaton, create another focus
    /// automaton.  So we need a means to recognize focus automata,
    /// and extract their true context, not just their visible
    /// context.
    template <Automaton Aut>
    struct real_context_impl;

    /// For a focus automaton, its genuine context (not the visible
    /// one), and for all the other automata, their context.
    template <Automaton Aut>
    auto
    real_context(const Aut& aut)
      -> decltype(real_context_impl<Aut>::context(aut));

    template <Automaton Aut>
    struct real_context_impl
    {
      static auto context(const Aut& aut)
        -> decltype(aut->context())
      {
        return aut->context();
      }
    };

    template <std::size_t Tape, Automaton Aut>
    struct real_context_impl<focus_automaton<Tape, Aut>>
    {
      static auto context(const focus_automaton<Tape, Aut>& aut)
        -> decltype(aut->full_context())
      {
        return aut->full_context();
      }
    };

    /// Be recursive on automaton wrappers.
    //
    // FIXME: try to recurse on all automaton decorator types without
    // listing them.
    template <Automaton Aut>
    struct real_context_impl<automaton_decorator<Aut>>
    {
      static auto context(const automaton_decorator<Aut>& aut)
        -> decltype(real_context(aut->strip()))
      {
        return real_context(aut->strip());
      }
    };

    template <Automaton Aut>
    auto
    real_context(const Aut& aut)
      -> decltype(real_context_impl<Aut>::context(aut))
    {
      return real_context_impl<Aut>::context(aut);
    }
  }

  /// Create an empty, mutable, automaton, based on another one.
  ///
  /// To this end, each automaton type provides a fresh_automaton_t.
  /// In the case of mutable_automaton, it's simply mutable_automaton.
  /// In the case of a transpose_automaton<Aut>, it's
  /// transpose_automaton (of the fresh_automaton of Aut) so that we
  /// also create a transposed automaton.  In the case of decorator,
  /// it's the base type, as we don't copy decorations.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>>
  AutOut
  make_fresh_automaton(const AutIn& model)
  {
    // FIXME: here, we need a means to convert the given input context
    // (e.g. letter -> B) into the destination one (e.g., letter ->
    // Q).  The automaton constructor wants the exact context type.
    return make_shared_ptr<AutOut>(detail::real_context(model));
  }


  /*------------------.
  | copy(automaton).  |
  `------------------*/

  namespace detail
  {
    /// Copy an automaton.
    ///
    /// \tparam AutIn
    ///    The type of the automaton to copy.
    /// \tparam AutOut
    ///    The type of the resulting automaton.
    /// \pre AutIn <: AutOut.
    template <Automaton AutIn, Automaton AutOut = AutIn>
    class copier
    {
    public:
      using in_automaton_t = AutIn;
      using out_automaton_t = AutOut;

      using in_state_t = state_t_of<in_automaton_t>;
      using out_state_t = state_t_of<out_automaton_t>;
      /// input state -> output state.
      using state_map_t = std::unordered_map<in_state_t, out_state_t>;

    private:
      /// Input automaton.
      in_automaton_t in_;
      /// Output automaton.
      out_automaton_t out_;
      /// whether the input automaton is in normal form and never has
      /// two transitions with same (src, label, dst).
      bool safe_ = true;
      /// input state -> output state.
      state_map_t out_state_;

    public:
      /// Prepare for an automaton full/partial duplication.
      ///
      /// \param in     input automaton, to copy
      /// \param out    output automaton, the destination
      /// \param safe   whether the input automaton is in normal form
      ///               and never has two transitions with same
      ///               (src, label, dst).
      copier(const in_automaton_t& in, out_automaton_t& out,
             bool safe = true)
        : in_(in)
        , out_(out)
        , safe_(safe)
        , out_state_{{in_->pre(),  out_->pre()},
                     {in_->post(), out_->post()}}
      {}

      /// Copy some states, and some transitions.
      ///
      /// \param keep_state  a predicate to recognize states to keep
      /// \param keep_trans  a predicate to recognize transitions to keep
      ///
      /// Transitions are kept only if both criteria are satisfied:
      /// the transition is kept, _and_ its source and destination too.
      template <typename KeepState, typename KeepTrans>
      void operator()(KeepState keep_state, KeepTrans keep_trans)
      {
        // Copy the states.  We cannot iterate on the transitions
        // only, as we would lose the states without transitions.  And
        // this way, we keep the states in the same order.
        for (auto s: in_->states())
          if (keep_state(s))
            out_state_[s] = out_->new_state();

        for (auto t : all_transitions(in_))
          if (keep_trans(t))
            {
              auto src = out_state_.find(in_->src_of(t));
              auto dst = out_state_.find(in_->dst_of(t));
              if (src != out_state_.end() && dst != out_state_.end())
                {
                  if (safe_)
                    out_->new_transition_copy(src->second, dst->second,
                                              in_, t);
                  else
                    out_->add_transition_copy(src->second, dst->second,
                                              in_, t);
                }
            }
      }

      /// Copy some transitions, and their corresponding states.
      ///
      /// \param ts     the transitions to keep.
      template <typename Transitions>
      auto operator()(const Transitions& ts)
        -> decltype(ts[0] == in_->null_transition(), void())
      {
        for (auto t : ts)
          operator()(t);
      }

      /// Copy one transition, and its corresponding states.
      ///
      /// \param t      the transition to copy.
      void operator()(const transition_t_of<in_automaton_t>& t)
      {
        auto src = state(in_->src_of(t));
        auto dst = state(in_->dst_of(t));
        if (safe_)
          out_->new_transition_copy(src, dst, in_, t);
        else
          out_->add_transition_copy(src, dst, in_, t);
      }

      /// Copy all the states, and all the transitions.
      void operator()()
      {
        operator()([](state_t_of<in_automaton_t>) { return true; },
                   [](transition_t_of<in_automaton_t>) { return true; });
      }

      /// A map from original state to result state.
      const state_map_t& state_map() const
      {
        return out_state_;
      }

      /// A map from original state to result state.
      state_map_t& state_map()
      {
        return out_state_;
      }

    private:
      /// The out state corresponding to the in-state \a s.
      /// If unknown, allocate it.
      out_state_t state(const in_state_t& s)
      {
        out_state_t res;
        auto i = out_state_.find(s);
        if (i == std::end(out_state_))
          res = out_state_.emplace(s, out_->new_state()).first->second;
        else
          res = i->second;
        return res;
      }
    };
  }


  /// Build an automaton copier.
  template <Automaton AutIn, Automaton AutOut>
  detail::copier<AutIn, AutOut>
  make_copier(const AutIn& in, AutOut& out, bool safe = true)
  {
    return {in, out, safe};
  }


  /// Copy selected states and transitions of an automaton.
  /// \pre AutIn <: AutOut.
  template <Automaton AutIn, Automaton AutOut,
            typename KeepState, typename KeepTrans>
  void
  copy_into(const AutIn& in, AutOut& out,
            KeepState keep_state, KeepTrans keep_trans)
  {
    auto copy = make_copier(in, out);
    copy(keep_state, keep_trans);
  }

  /// Copy the selected states an automaton.
  /// \pre AutIn <: AutOut.
  template <Automaton AutIn, Automaton AutOut, typename KeepState>
  void
  copy_into(const AutIn& in, AutOut& out, KeepState keep_state)
  {
    return copy_into(in, out, keep_state,
                     [](transition_t_of<AutIn>) { return true; });
  }


  /// Copy an automaton.
  /// \pre AutIn <: AutOut.
  template <Automaton AutIn, Automaton AutOut>
  void
  copy_into(const AutIn& in, AutOut& out)
  {
    return copy_into(in, out,
                     [](state_t_of<AutIn>) { return true; },
                     [](transition_t_of<AutIn>) { return true; });
  }


  /// Copy an automaton.
  /// \pre AutIn <: AutOut.
  template <Automaton AutIn, Automaton AutOut>
  void
  copy_into(const AutIn& in, AutOut& out, bool safe)
  {
    auto copy = make_copier(in, out, safe);
    copy([](state_t_of<AutIn>) { return true; },
         [](transition_t_of<AutIn>) { return true; });
  }


  /// A copy of \a input keeping only its states that are accepted by
  /// \a keep_state, and transitions accepted by \a keep_trans.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>,
            typename KeepState, typename KeepTrans>
  auto
  copy(const AutIn& input, KeepState keep_state, KeepTrans keep_trans)
    -> decltype(keep_state(input->null_state()),
                keep_trans(input->null_transition()),
                make_fresh_automaton<AutIn, AutOut>(input))
  {
    auto res = make_fresh_automaton<AutIn, AutOut>(input);
    ::vcsn::copy_into(input, res, keep_state, keep_trans);
    return res;
  }


  /// A copy of \a input.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>,
            typename KeepState>
  auto
  copy(const AutIn& input, KeepState keep_state)
    -> decltype(keep_state(input->null_state()),
                make_fresh_automaton<AutIn, AutOut>(input))
  {
    return ::vcsn::copy<AutIn, AutOut>(
             input,
             keep_state,
             [](transition_t_of<AutIn>) { return true; }
    );
  }


  /// A copy of \a input.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>>
  AutOut
  copy(const AutIn& input)
  {
    return ::vcsn::copy<AutIn, AutOut>(
             input,
             [](state_t_of<AutIn>) { return true; },
             [](transition_t_of<AutIn>) { return true; }
    );
  }


  /// A copy of \a input keeping only its states that are members of
  /// \a states.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>,
            typename States>
  auto
  copy(const AutIn& input, const States& ss)
    -> decltype(*ss.begin() == input->null_state(),
                make_fresh_automaton<AutIn, AutOut>(input))
  {
    return ::vcsn::copy<AutIn, AutOut>
      (input,
       [&ss](state_t_of<AutIn> s) { return has(ss, s); });
  }


  /// A copy of \a input keeping only its states that are members of
  /// container \a ss, and transitions that are members of container
  /// \a ts.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>,
            typename States, typename Trans>
  auto
  copy(const AutIn& input, const States& ss, const Trans& ts)
    -> decltype(*ss.begin() == input->null_state(),
                *ts.begin() == input->null_transition(),
                make_fresh_automaton<AutIn, AutOut>(input))
  {
    return ::vcsn::copy<AutIn, AutOut>
      (input,
       [&ss](state_t_of<AutIn> s)      { return has(ss, s); },
       [&ts](transition_t_of<AutIn> t) { return has(ts, t); });
  }


  /// A copy of \a input keeping only its transitions that are members
  /// of \a ts, and the states on which these transitions depend.
  template <Automaton AutIn,
            Automaton AutOut = fresh_automaton_t_of<AutIn>,
            typename Transitions>
  auto
  copy(const AutIn& input, const Transitions& ts)
    -> decltype(*ts.begin() == input->null_transition(),
                make_fresh_automaton<AutIn, AutOut>(input))
  {
    auto res = make_fresh_automaton<AutIn, AutOut>(input);
    auto copy = make_copier(input, res);
    copy(ts);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (copy).
      template <Automaton Aut, typename Ctx>
      automaton
      copy_convert(const automaton& aut, const context& ctx)
      {
        const auto& a = aut->as<Aut>();
        const auto& c = ctx->as<Ctx>();
        auto res = make_mutable_automaton(c);
        ::vcsn::copy_into(a, res);
        return res;
      }

      /// Bridge.
      template <Automaton Aut>
      automaton
      copy(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::copy(a);
      }
    }
  }

  /*--------------------.
  | copy(expression).   |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (copy).
      template <typename ExpSet, typename Context, typename Identities>
      expression
      copy_expression(const expression& exp,
                      const context& ctx, rat::identities ids)
      {
        const auto& r = exp->as<ExpSet>();
        const auto& c = ctx->as<Context>();
        const auto& rs = make_expressionset(c, ids);

        return {rs, ::vcsn::rat::copy(r.valueset(),
                                      rs,
                                      r.value())};
      }
    }
  }



} // namespace vcsn
