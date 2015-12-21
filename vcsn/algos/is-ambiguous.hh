#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/lightest-path.hh>
#include <vcsn/algos/conjunction.hh> // conjunction
#include <vcsn/algos/scc.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/label.hh>

namespace vcsn
{

  /*---------------.
  | is_ambiguous.  |
  `---------------*/

  /// Whether an automaton is ambiguous.
  ///
  /// \param[in] aut        the automaton
  /// \param[out] witness   if ambiguous, a pair of "ambiguous" states.
  /// \returns whether ambiguous.
  template <Automaton Aut>
  bool is_ambiguous(const Aut& aut,
                    std::tuple<state_t_of<Aut>, state_t_of<Aut>>& witness)
  {
    auto prod = conjunction(aut, aut);
    // Check if there useful states outside of the diagonal.  Since
    // the conjunction is accessible, check only for coaccessibles states.
    auto coaccessible = coaccessible_states(prod);
    for (const auto& o: prod->origins())
      if (std::get<0>(o.second) != std::get<1>(o.second)
          && has(coaccessible, o.first))
        {
          witness = o.second;
          return true;
        }
    return false;
  }

  template <Automaton Aut>
  bool is_ambiguous(const Aut& aut)
  {
    std::tuple<state_t_of<Aut>, state_t_of<Aut>> dummy;
    return is_ambiguous(aut, dummy);
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
    std::tuple<state_t_of<Aut>, state_t_of<Aut>> witness;
    require(is_ambiguous(aut, witness),
            "automaton is unambiguous");
    const auto& ls = *aut->labelset();
    // Find the shortest word from initial to the witness.
    auto s = std::get<0>(witness);

    auto pre_to_s = path_monomial(aut, lightest_path(aut, aut->pre(), s),
                                  aut->pre(), s);
    auto s_to_post = path_monomial(aut, lightest_path(aut, s, aut->post()),
                                   s, aut->post());
    require(pre_to_s, "ambiguous_word: did not find monomial");
    require(s_to_post, "ambiguous_word: did not find monomial");

    return ls.mul((*pre_to_s).first, (*s_to_post).first);
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
        return make_label(make_wordset(*a->labelset()), word);
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
    // Find all strongly connected components.
    const auto& coms = strong_components(aut,
                                         scc_algo_t::tarjan_iterative);
    if (getenv("VCSN_DEBUG"))
      {
        std::cerr << "number states of automaton: " <<
          aut->num_states() << std::endl;
        std::cerr << "number components: " <<
          coms.size() << std::endl;
      }

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
  /// Precondition: aut is a strongly connected component.
  template <Automaton Aut>
  bool is_cycle_ambiguous_scc(const Aut& aut)
  {
    auto prod = conjunction(aut, aut);
    const auto& coms = strong_components(prod,
                                         scc_algo_t::tarjan_iterative);
    const auto& origins = prod->origins();
    // In one SCC of prod = aut & aut, if there exist two states (s0,
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
