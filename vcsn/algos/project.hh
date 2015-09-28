#pragma once

#include <vcsn/labelset/tupleset.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/focus.hh>

namespace vcsn
{
  template <std::size_t Tape, typename Aut>
  auto project(const Aut& aut)
    -> mutable_automaton<detail::focus_context<Tape, context_t_of<Aut>>>
  {
    static_assert(Tape < labelset_t_of<Aut>::size(),
                  "project: invalid tape number");
    auto res = make_mutable_automaton(detail::make_focus_context<Tape>(aut->context()));
    copy_into(focus<Tape>(aut), res, false);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Tape>
      automaton
      project(const automaton& aut, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(vcsn::project<Tape::value>(a));
      }
    }
  }
}
