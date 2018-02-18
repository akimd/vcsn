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
    /// Whether an automaton is ambiguous.
    template <Automaton Aut>
    struct is_ambiguous_impl
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "is_ambiguous: requires free labelset");
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
      {}

      /// Whether an automaton is ambiguous.
      ///
      /// \returns whether ambiguous.
      bool operator()()
      {
        // Check if there useful states outside of the diagonal.
        // Since the conjunction is accessible, check only for
        // coaccessibles states.
        auto coaccessible = coaccessible_states(conj_);
        for (const auto& o: conj_->origins())
          if (std::get<0>(o.second) != std::get<1>(o.second)
              && has(coaccessible, o.first))
            {
              witness_ = o.first;
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
    static_assert(labelset_t_of<Aut>::is_free(),
                  "is_cycle_ambiguous: requires free labelset");
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
    const auto& origins = conj->origins();
    // In one SCC of conj = aut & aut, if there exist two states (s0,
    // s0) (on the diagonal) and (s1, s2) with s1 != s2 (off the
    // diagonal) then aut has two cycles with the same label:
    // s0->s1->s0 and s0->s2->s0.
    for (const auto& c : coms)
      {
        bool on = false;
        bool off = false;
        for (auto s : c)
          {
            auto p = origins.at(s);
            if (std::get<0>(p) == std::get<1>(p))
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
