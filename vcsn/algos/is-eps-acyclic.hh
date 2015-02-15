#pragma once

#include <unordered_map>

#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/core/fwd.hh>

namespace vcsn
{
  namespace detail
  {

    template <typename Aut,
              bool has_one = context_t_of<Aut>::has_one()>
    struct epsilon_acyclic;

    /// Detect epsilon-circuits.
    ///
    /// In this algorithm, only epsilon-transitions are considered.
    template <typename Aut>
    struct epsilon_acyclic<Aut, true>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      std::unordered_map<state_t, char> tag;
      /*
         tag gives the status of the state s;
         if s is not in the map, the state has not been reached yet;
         if tag[s]='u', the status is unknown, the graph reachable from s is
           under exploration
         if tag[s]='o', the status is "ok": there is no eps-circuit accessible
           from s
         if tag[s]='c', there is an eps-circuit accessible from s
         */

      const automaton_t& aut_;
      label_t empty_word;

      // Return true if an epsilon-circuit is accessible from s by
      // epsilon-transitions.
      bool has_epsilon_circuit(state_t s)
      {
        auto it = tag.find(s);
        if (it == tag.end())
        {
          tag[s] = 'u';
          for (auto t : aut_->out(s, empty_word))
            if (has_epsilon_circuit(aut_->dst_of(t)))
            {
              tag[s] = 'c';
              return true;
            }
          tag[s] = 'o';
          return false;
        }

        // Switch with respect to tag[s].
        switch (it->second)
        {
          case 'u':
            // s is reached while we are exploring successors of s:
            // there is a circuit.
            tag[s] = 'c';
            return true;
            // Otherwise the graph reachable from s has already been explored.
          case 'o':
            return false;
          default: //'c'
            return true;
        }
      }

      epsilon_acyclic(const automaton_t& aut)
        : aut_(aut)
        , empty_word(aut->labelset()->one())
      {
      }

      bool is_eps_acyclic()
      {
        for (auto s : aut_->states())
          if (has_epsilon_circuit(s))
            return false;
        return true;
      }
    };

    template <typename Aut>
    struct epsilon_acyclic<Aut, false>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;

      constexpr epsilon_acyclic(const automaton_t&)
      {}

      static constexpr bool is_eps_acyclic()
      {
        return true;
      }
    };
  }

  template <typename Aut>
  ATTRIBUTE_CONST
  bool is_eps_acyclic(const Aut& aut)
  {
    detail::epsilon_acyclic<Aut> t{aut};
    return t.is_eps_acyclic();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool is_eps_acyclic(const automaton& aut)
      {
        return is_eps_acyclic(aut->as<Aut>());
      }
    }
  }
} // namespace vcsn
