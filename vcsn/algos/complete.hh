#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/is-free.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/unordered_set.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(complete);

  /// Complete \a aut and return it.
  template <Automaton Aut>
  Aut&
  complete_here(Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_letterized(),
                  "complete: requires letterized labelset");
    require(is_free(aut),
            "complete: requires free automaton");

    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using letter_t = typename labelset_t_of<automaton_t>::letter_t;

    // A sink state, to allocate if needed.
    state_t sink = aut->null_state();
    const auto& ls = *aut->labelset();

    if (aut->num_initials() == 0)
      {
        sink = aut->new_state();
        aut->set_initial(sink);
      }

    auto num_gens = ls.generators().size();

    // The outgoing labels of a state.
    auto labels_met = std::unordered_set<letter_t>{};
    for (auto st : aut->states())
      if (st != sink)
        {
          labels_met.clear();
          for (auto tr : out(aut, st))
            labels_met.insert(aut->label_of(tr));

          if (labels_met.size() < num_gens)
            for (auto letter : ls.generators())
              if (!has(labels_met, letter))
                {
                  if (sink == aut->null_state())
                    sink = aut->new_state();
                  aut->new_transition(st, sink, letter);
                }
        }

    // Sink is created in two different cases, be careful if you want
    // to factor.
    if (sink != aut->null_state())
      for (auto letter : ls.generators())
        aut->new_transition(sink, sink, letter);

    aut->properties().update(complete_ftag{});
    return aut;
  }

  /// A complete copy of \a aut.
  template <Automaton Aut>
  auto
  complete(const Aut& aut)
    -> decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    complete_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      complete(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::complete(a);
      }
    }
  }

} // namespace vcsn
