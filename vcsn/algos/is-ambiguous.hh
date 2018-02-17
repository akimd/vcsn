#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/shortest.hh>
#include <vcsn/algos/conjunction.hh> // conjunction
#include <vcsn/algos/scc.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*---------------.
  | is_ambiguous.  |
  `---------------*/

  namespace detail
  {
    /// Retrieve the original state in case of insplit automaton.
    template <Automaton Aut2>
    auto real_state(const insplit_automaton<Aut2>& aut,
                     state_t_of<insplit_automaton<Aut2>> s)
    {
      return aut->origins().find(s)->second.first;
    }

    template <Automaton Aut2>
    auto real_state(const Aut2&, state_t_of<Aut2> s)
    {
      return s;
    }

    /// Whether the two states composing this state are the same.
    /// \param  aut  an automaton which is a self-conjunction
    /// \param  s    the state whose origin is checked for being (r, r).
    template <Automaton Conjunction>
    bool on_diagonal(const Conjunction& aut, state_t_of<Conjunction> s)
    {
      auto p = aut->origins().find(s)->second;
      return std::get<0>(p)
        == real_state(std::get<1>(aut->auts_), std::get<1>(p));
    }

    /// Whether an automaton is ambiguous.
    template <Automaton Aut>
    struct is_ambiguous_impl
    {
      using automaton_t = Aut;
      using conjunction_t
        = decltype(conjunction(std::declval<automaton_t>(),
                               std::declval<automaton_t>()));
      using word_t = word_t_of<automaton_t>;

      /// Constructor
      /// \param aut the automaton to study.
      is_ambiguous_impl(const automaton_t& aut)
        // FIXME: this product should not take weights into account!
        : conj_{conjunction(aut, aut)}
      {
        require(is_free(aut),
                "is_ambiguous: requires free automaton");
      }

      /// Whether an automaton is ambiguous.
      ///
      /// \returns whether ambiguous.
      bool operator()()
      {
        // Check if there useful states outside of the diagonal.
        // Since the conjunction is accessible, check only for
        // coaccessibles states.
        auto coaccessible = coaccessible_states(conj_);
        for (const auto s: conj_->states())
          if (!on_diagonal(conj_, s) && has(coaccessible, s))
            {
              witness_ = s;
              return true;
            }
        return false;
      }

      /// A witness of ambiguity.
      /// \pre this object proved that the automaton is ambiguous.
      word_t ambiguous_word() const
      {
        require(witness_ != conj_->null_state(),
                "ambiguous_word: automaton is unambiguous, "
                "or has not been tested, for ambiguity");
        const auto ls = make_wordset(*conj_->labelset());

        auto p1 = shortest(conj_, conj_->pre(), witness_);
        auto p2 = shortest(conj_, witness_, conj_->post());

        assert(!p1.empty() || !p2.empty()
               || !"ambiguous_word: did not find an ambiguous word");
        return ls.mul(p1.empty() ? ls.one() : p1.begin()->first,
                      p2.empty() ? ls.one() : p2.begin()->first);
      }

      /// The self-conjunction of the input automaton.
      conjunction_t conj_;
      /// State index in the conjunction of a state that is not on the
      /// diagonal.
      state_t_of<conjunction_t> witness_ = conj_->null_state();
    };
  }

  /// Whether an automaton is ambiguous.
  ///
  /// \param  aut        the automaton.
  /// \returns whether ambiguous.
  template <Automaton Aut>
  bool is_ambiguous(const Aut& aut)
  {
    auto is_ambiguous = detail::is_ambiguous_impl<Aut>{aut};
    return is_ambiguous();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_ambiguous(const automaton& aut)
      {
        return is_ambiguous(aut->as<Aut>());
      }
    }
  }


  /*-----------------.
  | ambiguous_word.  |
  `-----------------*/

  template <Automaton Aut>
  word_t_of<Aut> ambiguous_word(const Aut& aut)
  {
    auto is_ambiguous = detail::is_ambiguous_impl<Aut>{aut};
    require(is_ambiguous(), "ambiguous_word: automaton is unambiguous");
    return is_ambiguous.ambiguous_word();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      label
      ambiguous_word(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto word = vcsn::ambiguous_word(a);
        return {make_wordset(*a->labelset()), word};
      }
    }
  }



  /*---------------------.
  | is_cycle_ambiguous.  |
  `---------------------*/

  /// Whether \a aut is cycle-ambiguous.
  template <Automaton Aut>
  bool is_cycle_ambiguous(const Aut& aut)
  {
    require(is_free(aut),
            "is_cycle_ambiguous: requires free automaton");
    // Find all strongly connected components.
    const auto& coms = strong_components(aut,
                                         scc_algo_t::tarjan_iterative);
    // Check each component if it is cycle-ambiguous.
    if (coms.size() == 1)
      return is_cycle_ambiguous_scc(aut);
    for (const auto &c : coms)
      {
        const auto& a = aut_of_component(c, aut);
        if (is_cycle_ambiguous_scc(a))
          return true;
      }
    return false;
  }

  /// Whether \a aut is cycle-ambiguous.
  ///
  /// \pre aut is a strongly connected component.
  template <Automaton Aut>
  bool is_cycle_ambiguous_scc(const Aut& aut)
  {
    auto conj = conjunction(aut, aut);
    const auto& coms = strong_components(conj,
                                         scc_algo_t::tarjan_iterative);
    // In one SCC of conj = aut & aut, if there exist two states (s0,
    // s0) (on the diagonal) and (s1, s2) with s1 != s2 (off the
    // diagonal) then aut has two cycles with the same label:
    // s0->s1->s0 and s0->s2->s0.
    for (const auto& c : coms)
      {
        bool on = false;
        bool off = false;
        for (const auto s : c)
          {
            if (on_diagonal(conj, s))
              on = true;
            else
              off = true;
            if (on && off)
              return true;
          }
      }
    return false;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool
      is_cycle_ambiguous(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::is_cycle_ambiguous(a);
      }
    }
  }
}
