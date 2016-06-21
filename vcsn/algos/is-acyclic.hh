#pragma once

#include <unordered_map>

#include <boost/optional.hpp>

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
    class is_acyclic_impl
    {
    public:
      using automaton_t = std::remove_cv_t<Aut>;
      using state_t = state_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      is_acyclic_impl(const automaton_t& aut,
                      boost::optional<label_t> label = {})
        : aut_(aut)
        , one_(label)
      {}

      /// Whether the automaton is acyclic.
      bool operator()()
      {
        for (auto s : aut_->states())
          if (has_circuit_(s))
            return false;
        return true;
      }

    private:
      /// Return true if an circuit is accessible from s.
      bool has_circuit_(state_t s)
      {
        auto it = tag_.find(s);
        if (it == tag_.end())
          {
            tag_[s] = unknown;
            // Code duplication because `out(aut_, s, *one)` and
            // `out(aut_, s)` have different return types.
            if (one_)
              {
                for (auto t : out(aut_, s, *one_))
                  if ((aut_->dst_of(t) == s && aut_->label_of(t) == *one_)
                      || has_circuit_(aut_->dst_of(t)))
                    {
                      tag_[s] = circuit;
                      return true;
                    }
              }
            else
              {
                for (auto t : out(aut_, s))
                  if (aut_->dst_of(t) == s || has_circuit_(aut_->dst_of(t)))
                    {
                      tag_[s] = circuit;
                      return true;
                    }
              }
            tag_[s] = ok;
            return false;
          }

        // Switch with respect to tag_[s].
        switch (it->second)
          {
          case unknown:
            // s is reached while we are exploring successors of s:
            // there is a circuit.
            tag_[s] = circuit;
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
      std::unordered_map<state_t, status> tag_;

      automaton_t aut_;
      boost::optional<label_t> one_;
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
    auto is_acyclic = detail::is_acyclic_impl<Aut>{aut, aut->labelset()->one()};
    return is_acyclic();
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
    auto is_acyclic = detail::is_acyclic_impl<Aut>{aut};
    return is_acyclic();
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
