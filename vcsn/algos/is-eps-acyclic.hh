#pragma once

#include <unordered_map>

#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/core/fwd.hh>

namespace vcsn
{
  namespace detail
  {

    template <Automaton Aut,
              bool has_one = context_t_of<Aut>::has_one()>
    struct epsilon_acyclic;

    /// Detect epsilon-circuits.
    ///
    /// In this algorithm, only epsilon-transitions are considered.
    template <Automaton Aut>
    struct epsilon_acyclic<Aut, true>
    {
      using automaton_t = std::remove_cv_t<Aut>;
      using state_t = state_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      epsilon_acyclic(const automaton_t& aut)
        : aut_(aut)
      {}

      bool is_eps_acyclic()
      {
        for (auto s : aut_->states())
          if (has_spontaneous_circuit(s))
            return false;
        return true;
      }

      // Return true if an epsilon-circuit is accessible from s by
      // epsilon-transitions.
      bool has_spontaneous_circuit(state_t s)
      {
        auto it = tag.find(s);
        if (it == tag.end())
        {
          tag[s] = unknown;
          for (auto t : out(aut_, s, one))
            if (has_spontaneous_circuit(aut_->dst_of(t)))
            {
              tag[s] = circuit;
              return true;
            }
          tag[s] = ok;
          return false;
        }

        // Switch with respect to tag[s].
        switch (it->second)
        {
          case unknown:
            // s is reached while we are exploring successors of s:
            // there is a circuit.
            tag[s] = circuit;
            return true;
          case ok:
            // Otherwise the graph reachable from s has already been explored.
            return false;
          case circuit:
            return true;
        }
        BUILTIN_UNREACHABLE();
      }

      enum status
        {
          /// The graph reachable from s is under exploration.
          unknown,
          /// There is no eps-circuit accessible from s.
          ok,
          /// There is an eps-circuit accessible from s.
          circuit,
        };

      // States not in the map have not been reached yet.
      std::unordered_map<state_t, status> tag;

      automaton_t aut_;
      label_t one = aut_->labelset()->one();
    };

    template <Automaton Aut>
    struct epsilon_acyclic<Aut, false>
    {
      using automaton_t = std::remove_cv_t<Aut>;

      constexpr epsilon_acyclic(const automaton_t&)
      {}

      static constexpr bool is_eps_acyclic()
      {
        return true;
      }
    };
  }

  template <Automaton Aut>
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
      template <Automaton Aut>
      bool is_eps_acyclic(const automaton& aut)
      {
        return is_eps_acyclic(aut->as<Aut>());
      }
    }
  }
} // namespace vcsn
