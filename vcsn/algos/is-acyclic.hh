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

    /// Detect circuits.
    template <Automaton Aut>
    struct acyclic
    {
      using automaton_t = std::remove_cv_t<Aut>;
      using state_t = state_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      acyclic(const automaton_t& aut, boost::optional<label_t> label = {})
        : aut_(aut)
        , one(label)
      {}

      bool is_acyclic()
      {
        for (auto s : aut_->states())
          if (has_circuit(s))
            return false;
        return true;
      }

      /// Return true if an circuit is accessible from s.
      bool has_circuit(state_t s)
      {
        auto it = tag.find(s);
        if (it == tag.end())
          {
            tag[s] = unknown;
            // This code duplication is caused by the fact that
            // out(aut_, s, *one) has a return type different from out(aut_, s).
            if (one != boost::none)
              {
                for (auto t : out(aut_, s, *one))
                  if ((aut_->dst_of(t) == s && aut_->label_of(t) == *one)
                      || has_circuit(aut_->dst_of(t)))
                    {
                      tag[s] = circuit;
                      return true;
                    }
              }
            else
              {
                for (auto t : out(aut_, s))
                  if (aut_->dst_of(t) == s || has_circuit(aut_->dst_of(t)))
                    {
                      tag[s] = circuit;
                      return true;
                    }
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
          /// There is no circuit accessible from s.
          ok,
          /// There is an circuit accessible from s.
          circuit,
        };

      // States not in the map have not been reached yet.
      std::unordered_map<state_t, status> tag;

      automaton_t aut_;
      boost::optional<label_t> one;
    };
  }

  /// Detect epsilon-circuits.
  ///
  /// In this algorithm, only epsilon-transitions are considered.
  template <Automaton Aut>
  ATTRIBUTE_CONST
  std::enable_if_t<context_t_of<Aut>::has_one(), bool>
  is_eps_acyclic(const Aut& aut)
  {
    auto t = detail::acyclic<Aut>{aut, aut->labelset()->one()};
    return t.is_acyclic();
  }

  template <Automaton Aut>
  ATTRIBUTE_CONST
  std::enable_if_t<!context_t_of<Aut>::has_one(), bool>
  is_eps_acyclic(const Aut&)
  {
    return true;
  }

  template <Automaton Aut>
  ATTRIBUTE_CONST
  bool
  is_acyclic(const Aut& aut)
  {
    auto t = detail::acyclic<Aut>{aut};
    return t.is_acyclic();
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
