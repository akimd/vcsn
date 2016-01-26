#pragma once

#include <vcsn/algos/epsilon-remover-separate.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/core/automaton.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    /// Build a "lazy proper" automaton from the input with nullable labels.
    /// The resulting automaton has the `proper_context` matching the context
    /// of the input.
    ///
    /// We use the epsilon-remover-separate algorithm to build two separate
    /// automata: the spontaneous one and the proper one. The methods of
    /// lazy_proper_automaton are forwarded to the proper automaton of the
    /// separate remover algorithm.
    ///
    /// Since the remover implementation works on incoming transitions and we
    /// want to be lazy on outgoing transitions to have a forward automaton, we
    /// transpose the input automaton before passing it to the remover, and we
    /// transpose its proper part again when adding it as the underlying
    /// automaton.
    template <Automaton Aut,
              bool has_one = labelset_t_of<Aut>::has_one()>
    class lazy_proper_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut,
                                   detail::proper_context<context_t_of<Aut>>>>
    {
    public:
      using in_automaton_t = Aut;
      using in_context_t = context_t_of<in_automaton_t>;

      using context_t = detail::proper_context<context_t_of<Aut>>;
      using automaton_t = fresh_automaton_t_of<Aut, context_t>;

      using self_t = lazy_proper_automaton_impl;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      using transpose_in_automaton_t = transpose_automaton<in_automaton_t>;

      static symbol sname()
      {
        static auto res = symbol{"lazy_proper_automaton<"
                                 + in_automaton_t::element_type::sname()
                                 + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt) const
      {
        o << "lazy_proper_automaton<";
        this->aut_->print_set(o, fmt);
        return o << '>';
      }

      /// We cannot initialize super_t with the input automaton, we have to
      /// call remover_() first with the transposed input, then assign the
      /// proper automaton from the remover to this->aut_.
      lazy_proper_automaton_impl(const in_automaton_t& a, bool prune = true)
        : super_t(make_proper_context(a->context()))
        , remover_(transpose(a), prune)
      {
        this->aut_ = transpose(remover_.get_proper());
        proper_states_.emplace(this->post()); // post is already proper
        known_states_.emplace(this->pre()); // pre is always there
        known_states_.emplace(this->post()); // post is always there
      }

      /// Whether the given state has outgoing spontaneous transitions.
      bool state_has_spontaneous_out(state_t s) const
      {
        // We work on the transpose automaton, in and out are reversed
        return remover_.state_has_spontaneous_in(s);
      }

      /// Complete a state: find its outgoing transitions.
      ///
      /// Since remover_on can remove the state it's working on, and add
      /// outgoing transitions from its parents, we need to call it on all the
      /// successors of the state while they have outgoing spontaneous
      /// transitions.
      void complete_(state_t s) const
      {
        auto& self = const_cast<self_t&>(*this);

        bool has_removed = true;
        while (has_removed)
          {
            auto ts = vcsn::detail::all_out(this->aut_, s);
            auto todo = std::vector<state_t>(ts.size());
            std::transform(begin(ts), end(ts), begin(todo),
              [this](auto t){ return this->aut_->dst_of(t); });
            has_removed = false;
            for (auto succ : todo)
              if (state_has_spontaneous_out(succ))
                {
                  self.remover_.remover_on(succ);
                  has_removed = true;
                }
          }

        for (auto t : vcsn::detail::all_out(this->aut_, s))
          self.known_states_.emplace(this->aut_->dst_of(t));
        self.proper_states_.emplace(s);
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
        -> decltype(vcsn::detail::all_out(this->aut_, s))
      {
        if (is_lazy(s))
          complete_(s);
        return vcsn::detail::all_out(this->aut_, s);
      }

      bool is_lazy(state_t s) const
      {
        return !has(proper_states_, s);
      }

      /// All states including pre()/post().
      /// Guaranteed in increasing order.
      auto all_states() const
      {
        return all_states([](state_t){ return true; });
      }

      /// All states including pre()/post() that validate \a pred.
      /// Guaranteed in increasing order.
      template <typename Pred>
      auto all_states(Pred pred) const
      {
        return this->aut_->all_states(
          [this, pred](state_t s)
          {
            return pred(s) && has(known_states_, s);
          });
      }

      /// All states excluding pre()/post().
      /// Guaranteed in increasing order.
      auto states() const
      {
        return all_states(
          [this](state_t s)
          {
            return s != this->aut_->pre() && s != this->aut_->post();
          });
      }

    private:
      epsilon_remover_separate<transpose_in_automaton_t> remover_;

      /// States whose outgoing transitions are known.
      std::unordered_set<state_t> proper_states_;

      /// States we want to show ("wavefront", proper_states and their
      /// successors)
      std::unordered_set<state_t> known_states_;
    };

    /// Specialization for automata with non nullable context.
    /// We just forward everything. All states are strict.
    template <Automaton Aut>
    class lazy_proper_automaton_impl<Aut, false>
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      static symbol sname()
      {
        static auto res = symbol{"lazy_proper_automaton<"
                                 + automaton_t::element_type::sname()
                                 + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt) const
      {
        o << "lazy_proper_automaton<";
        this->aut_->print_set(o, fmt);
        return o << '>';
      }

      lazy_proper_automaton_impl(const automaton_t& a, bool)
        : super_t(a)
      {}
    };
  }

  template <Automaton Aut>
  using lazy_proper_automaton
    = std::shared_ptr<detail::lazy_proper_automaton_impl<Aut>>;
}
