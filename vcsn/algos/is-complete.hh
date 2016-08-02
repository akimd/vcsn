#pragma once

#include <set>

#include <vcsn/core/automaton.hh> // all_out
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  /// Whether \a aut is complete.
  /// \pre \a aut is LTL
  template <Automaton Aut>
  bool is_complete(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "is_complete: requires free labelset");

    if (aut->num_initials() == 0)
      return false;

    // FIXME: this is naive: an unordered_set and/or a bitset would
    // probably be more efficient.  See the benches.
    using label_set_t = std::set<typename labelset_t_of<Aut>::letter_t>;

    const auto& letters = aut->labelset()->generators();
    for (auto state : aut->states())
    {
      auto missing_letters
        = label_set_t{std::begin(letters), std::end(letters)};

      for (auto tr : all_out(aut, state))
        missing_letters.erase(aut->label_of(tr));

      if (!missing_letters.empty())
        return false;
    }

    return true;
  }

  /*------------------.
  | dyn::is-complete. |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_complete(const automaton& aut)
      {
        return is_complete(aut->as<Aut>());
      }
    }
  }
}
