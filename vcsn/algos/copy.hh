#pragma once

#include <unordered_map>

#include <vcsn/algos/fwd.hh> // focus_automaton.
#include <vcsn/core/fwd.hh>
#include <vcsn/core/rat/copy.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/expressionset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/unordered_set.hh>

namespace vcsn
{

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
    template <typename AutIn, typename AutOut>
    struct copier
    {
      using in_state_t = state_t_of<AutIn>;
      using out_state_t = state_t_of<AutOut>;

      copier(const AutIn& in, AutOut& out)
        : in_(in)
        , out_(out)
      {}

      template <typename KeepState, typename KeepTrans>
      void operator()(KeepState keep_state, KeepTrans keep_trans)
      {
        // Copy the states.  We cannot iterate on the transitions
        // only, as we would lose the states without transitions.
        out_state[in_->pre()] = out_->pre();
        out_state[in_->post()] = out_->post();
        for (auto s: in_->states())
          if (keep_state(s))
            out_state[s] = out_->new_state();

        for (auto t : in_->all_transitions())
          if (keep_trans(t))
            {
              auto src = out_state.find(in_->src_of(t));
              auto dst = out_state.find(in_->dst_of(t));
              if (src != out_state.end() && dst != out_state.end())
                out_->new_transition_copy(src->second, dst->second,
                                          in_, t);
            }
      }

      /// A map from result state to original state.
      using origins_t = std::map<out_state_t, in_state_t>;
      origins_t
      origins() const
      {
        origins_t res;
        for (const auto& p: out_state)
          res[p.second] = p.first;
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(const origins_t& orig, std::ostream& o)
      {
        o << "/* Origins.\n"
          << "    node [shape = box, style = rounded]\n";
        for (auto p : orig)
          if (2 <= p.first)
            o << "    " << p.first - 2
              << " [label = \"" << p.second - 2 << "\"]\n";
        o << "*/\n";
        return o;
      }

      /// Input automaton.
      const AutIn& in_;
      /// Output automaton.
      AutOut& out_;
      /// input state -> output state.
      std::unordered_map<in_state_t, out_state_t> out_state;
    };
  }

  /// Copy selected states and transitions of an automaton.
  /// \pre AutIn <: AutOut.
  template <typename AutIn, typename AutOut,
            typename KeepState, typename KeepTrans>
  inline void
  copy_into(const AutIn& in, AutOut& out,
            KeepState keep_state, KeepTrans keep_trans)
  {
    detail::copier<AutIn, AutOut> copy(in, out);
    return copy(keep_state, keep_trans);
  }

  /// Copy the selected states an automaton.
  /// \pre AutIn <: AutOut.
  template <typename AutIn, typename AutOut, typename KeepState>
  inline
  void
  copy_into(const AutIn& in, AutOut& out, KeepState keep_state)
  {
    return copy_into(in, out, keep_state,
                     [](transition_t_of<AutIn>) { return true; });
  }

  /// Copy an automaton.
  /// \pre AutIn <: AutOut.
  template <typename AutIn, typename AutOut>
  inline
  void
  copy_into(const AutIn& in, AutOut& out)
  {
    return copy_into(in, out,
                     [](state_t_of<AutIn>) { return true; },
                     [](transition_t_of<AutIn>) { return true; });
  }

  namespace detail
  {
    template <typename Aut>
    struct real_context_impl;

    template <typename Aut>
    auto
    real_context(const Aut& aut)
      -> decltype(real_context_impl<Aut>::context(aut));

    template <typename Aut>
    struct real_context_impl
    {
      static auto context(const Aut& aut)
        -> decltype(aut->context())
      {
        return aut->context();
      }
    };

    template <std::size_t Tape, typename Aut>
    struct real_context_impl<focus_automaton<Tape, Aut>>
    {
      static auto context(const focus_automaton<Tape, Aut>& aut)
        -> decltype(aut->full_context())
      {
        return aut->full_context();
      }
    };

    template <typename Aut>
    struct real_context_impl<permutation_automaton<Aut>>
    {
      static auto context(const permutation_automaton<Aut>& aut)
        -> decltype(real_context(aut->strip()))
      {
        return real_context(aut->strip());
      }
    };

    template <typename Aut>
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
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>>
  inline AutOut
  make_fresh_automaton(const AutIn& model)
  {
    // FIXME: here, we need a means to convert the given input context
    // (e.g. letter -> B) into the destination one (e.g., letter ->
    // Q).  The automaton constructor wants the exact context type.
    return make_shared_ptr<AutOut>(detail::real_context(model));
  }

  /// A copy of \a input keeping only its states that are accepted by
  /// \a keep_state.
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>,
            typename KeepState, typename KeepTrans>
  inline
  AutOut
  copy(const AutIn& input, KeepState keep_state, KeepTrans keep_trans)
  {
    auto res = make_fresh_automaton<AutIn, AutOut>(input);
    ::vcsn::copy_into(input, res, keep_state, keep_trans);
    return res;
  }

  /// A copy of \a input.
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>,
            typename KeepState>
  inline
  AutOut
  copy(const AutIn& input, KeepState keep_state)
  {
    return ::vcsn::copy<AutIn, AutOut>(
             input,
             keep_state,
             [](transition_t_of<AutIn>) { return true; }
    );
  }

  /// A copy of \a input.
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>>
  inline
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
  /// std::set \a keep.
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>>
  inline
  AutOut
  copy(const AutIn& input, const std::set<state_t_of<AutIn>>& keep)
  {
    return ::vcsn::copy<AutIn, AutOut>
      (input,
       [&keep](state_t_of<AutIn> s) { return has(keep, s); });
  }

  /// A copy of \a input keeping only its states that are members of
  /// std::unordered_set \a keep.
  template <typename AutIn,
            typename AutOut = fresh_automaton_t_of<AutIn>>
  inline
  AutOut
  copy(const AutIn& input, const std::unordered_set<state_t_of<AutIn>>& keep)
  {
    return ::vcsn::copy<AutIn, AutOut>
      (input,
       [&keep](state_t_of<AutIn> s) { return has(keep, s); });
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (copy).
      template <typename Aut, typename Ctx>
      inline
      automaton
      copy_convert(const automaton& aut, const context& ctx)
      {
        const auto& a = aut->as<Aut>();
        const auto& c = ctx->as<Ctx>();
        auto res = make_mutable_automaton(c);
        ::vcsn::copy_into(a, res);
        return make_automaton(res);
      }

      /// Bridge.
      template <typename Aut>
      inline
      automaton
      copy(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::copy(a));
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
      template <typename InExpSet, typename OutExpSet = InExpSet>
      inline
      expression
      copy_expression(const expression& exp, const expressionset& out_rs)
      {
        const auto& r = exp->as<InExpSet>();
        const auto& ors = out_rs->as<OutExpSet>().expressionset();

        return make_expression(ors,
                           ::vcsn::rat::copy(r.expressionset(), ors,
                                             r.expression()));
      }
    }
  }



} // namespace vcsn
