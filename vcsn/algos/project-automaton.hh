#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/focus.hh>
#include <vcsn/algos/project.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/dyn/name.hh> // integral_constant

namespace vcsn
{

  /*----------------------.
  | project(automaton).   |
  `----------------------*/

  template <std::size_t Tape, Automaton Aut>
  auto project(const Aut& aut)
    -> mutable_automaton<detail::project_context<Tape, context_t_of<Aut>>>
  {
    static_assert(Tape < labelset_t_of<Aut>::size(),
                  "project: invalid tape number");
    auto res = make_mutable_automaton(detail::project<Tape>(aut->context()));
    copy_into(focus<Tape>(aut), res, false);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Tape>
      automaton
      project(const automaton& aut, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return vcsn::project<Tape::value>(a);
      }
    }
  }
}
