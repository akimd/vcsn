#pragma once

#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/project.hh> // project
#include <vcsn/core/automaton.hh> // out
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant
#include <vcsn/misc/tuple.hh> // make_index_range

namespace vcsn
{

  /*-------------------.
  | focus_automaton.   |
  `-------------------*/

  namespace detail
  {
    template <Automaton A, typename I>
    struct hidden_label_type;

    template <Automaton Aut, std::size_t... I>
    struct hidden_label_type<Aut, index_sequence<I...>>
    {
      using ls_t = labelset_t_of<Aut>;
      using type = tupleset<typename ls_t::template valueset_t<I>...>;
    };


    /// Read-write on an automaton, that hides all tapes but one.
    template <std::size_t Tape, Automaton Aut>
    class focus_automaton_impl
      : public automaton_decorator<Aut,
                                   project_context<Tape, context_t_of<Aut>>>
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
      using context_t = project_context<Tape, full_context_t>;

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
           make_index_range_t<Tape + 1, std::tuple_size<full_label_t>::value>>;

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
        static auto res = symbol{"focus_automaton<"
                                 + std::to_string(Tape) + ", "
                                 + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "focus_automaton<" << Tape << ", ";
        aut_->print_set(o, fmt);
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
      labelset_ptr
      labelset() const
      {
        return ls_;
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
      std::enable_if_t<L::has_one(), res_label_t>
      hidden_one_(index_sequence<I...>) const
      {
        full_label_t l = aut_->labelset()->one();
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      std::enable_if_t<!L::has_one(), res_label_t>
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
        -> decltype(out(aut_, s))
      {
        return out(aut_, s);
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

      /// Whether this transition is present in the automaton.
      ///
      /// Necessary for transpose's has_transition to be available.
      bool
      has_transition(transition_t t) const
      {
        return aut_->has_transition(t);
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
      template <Automaton A>
      transition_t
      new_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          transition_t_of<A> t,
                          bool transpose = false)
      {
        return aut_->new_transition_copy(src, dst,
                                         aut->strip(), t, transpose);
      }

      /// Copy the full wrapped transition.
      template <Automaton A>
      transition_t
      add_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          transition_t_of<A> t,
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
             (state_t, state_t, label_t, weight_t) -> transition_t);
      DEFINE(add_transition,
             (state_t, state_t, label_t) -> transition_t);
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
      context_t context_ = project<Tape>(full_context());
      /// The apparent labelset. Caching it avoids difference in
      /// behavior with other automata. This example would not be
      /// working otherwise (with `focus` a focus automaton):
      /// `const auto& ls = *focus->labelset();`
      labelset_ptr ls_
        = std::make_shared<labelset_t>(aut_->labelset()->template set<Tape>());
    };
  }

  template <unsigned Tape, Automaton Aut>
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
      template <Automaton Aut, typename Tape>
      automaton
      focus(const automaton& aut, integral_constant)
      {
        auto& a = aut->as<Aut>();
        return vcsn::focus<Tape::value>(a);
      }
    }
  }
}
