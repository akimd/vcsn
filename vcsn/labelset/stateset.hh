#pragma once

#include <vcsn/ctx/traits.hh>

namespace vcsn
{

  /// State labelset.
  ///
  /// Used when representing states as labels of polynomials.
  template <Automaton Aut>
  class stateset
  {
  public:
    using automaton_t = Aut;
    using state_t = state_t_of<Aut>;

    stateset(const automaton_t& aut)
      : aut_(aut)
    {}

    using value_t = state_t;

    // So that we don't try to print ranges of states.
    static constexpr bool
    is_letterized()
    {
      return false;
    }

    using kind_t = void;
    static bool equal(state_t l, state_t r)
    {
      return l == r;
    }

    static bool less(state_t l, state_t r)
    {
      return l < r;
    }

    static size_t hash(state_t s)
    {
      return hash_value(s);
    }

    std::ostream&
    print(state_t s, std::ostream& out, format fmt = {}) const
    {
      return aut_->print_state_name(s, out, fmt);
    }

  private:
    automaton_t aut_;
  };
}
