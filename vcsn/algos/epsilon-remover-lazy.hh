#pragma once

#include <vcsn/algos/epsilon-remover-separate.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/ctx/traits.hh>
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
    template <typename Aut,
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
        static symbol res("lazy_proper_automaton<"
                          + in_automaton_t::element_type::sname() + '>');
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
            auto all_out = this->aut_->all_out(s);
            std::vector<state_t> todo(all_out.size());
            std::transform(begin(all_out), end(all_out), begin(todo),
              [this](auto t){ return this->aut_->dst_of(t); });
            has_removed = false;
            for (auto succ : todo)
              if (state_has_spontaneous_out(succ))
                {
                  self.remover_.remover_on(succ);
                  has_removed = true;
                }
          }

        for (auto t : this->aut_->all_out(s))
          self.known_states_.emplace(this->aut_->dst_of(t));
        self.proper_states_.emplace(s);
      }

      /// All the outgoing transitions.
      using super_t::all_out;
      auto all_out(state_t s) const
        -> decltype(this->aut_->all_out(s))
      {
        if (!state_is_strict(s))
          complete_(s);
        return this->aut_->all_out(s);
      }

      /// All the outgoing transitions satisfying the predicate.
      template <typename Pred>
      auto all_out(state_t s, Pred pred) const
        -> decltype(this->aut_->all_out(s, pred))
      {
        if (!state_is_strict(s))
          complete_(s);
        return this->aut_->all_out(s, pred);
      }

      // FIXME: clang workaround.
      struct label_equal_p
      {
        bool operator()(transition_t_of<self_t> t) const
        {
          return this->aut_.labelset()->equal(this->aut_.label_of(t), label_);
        }
        const self_t& aut_;
        // Capture by copy: in the case of the transpose_automaton, the
        // labels are transposed, so they are temporaries.
        label_t_of<self_t> label_;
      };

      // FIXME: clang workaround.
      struct not_to_post_p
      {
        bool operator()(transition_t_of<self_t> t) const
        {
          return aut_.dst_of(t) != aut_.post();
        }
        const self_t& aut_;
      };


      /// Indexes of visible transitions leaving state \a s.
      /// Invalidated by del_transition() and del_state().
      auto out(state_t s)
        -> decltype(this->all_out(s, not_to_post_p{*this}))
      {
        return this->all_out(s, not_to_post_p{*this});
      }

      /// Indexes of all transitions leaving state \a s on label \a l.
      /// Invalidated by del_transition() and del_state().
      auto out(state_t s, label_t_of<self_t> l)
        -> decltype(this->all_out(s, label_equal_p{*this, l}))
      {
        return this->all_out(s, label_equal_p{*this, l});
      }

      bool state_is_strict(state_t s) const
      {
        return has(proper_states_, s);
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
    template <typename Aut>
    class lazy_proper_automaton_impl<Aut, false>
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      static symbol sname()
      {
        static symbol res("lazy_proper_automaton<"
                          + automaton_t::element_type::sname() + '>');
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
      {
      }

      bool state_is_strict(state_t)
      {
        return true;
      }
    };
  }

  template <typename Aut>
      using lazy_proper_automaton
      = std::shared_ptr<detail::lazy_proper_automaton_impl<Aut>>;
}
