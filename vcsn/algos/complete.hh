#ifndef VCSN_ALGOS_COMPLETE_HH
# define VCSN_ALGOS_COMPLETE_HH

# include <queue>
# include <unordered_map>

# include <vcsn/algos/copy.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/unordered_set.hh>

namespace vcsn
{
  /// Complete \a aut and return it.
  template <typename Aut>
  Aut&
  complete_here(Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "requires free labelset");

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

    // The outgoing labels of a state.
    std::unordered_set<letter_t> labels_met;
    for (auto st : aut->states())
      if (st != sink)
        {
          for (auto tr : aut->out(st))
            labels_met.insert(aut->label_of(tr));

          for (auto letter : ls.genset())
            if (!has(labels_met, letter))
              {
                if (sink == aut->null_state())
                  sink = aut->new_state();
                aut->new_transition(st, sink, letter);
              }

          labels_met.clear();
        }

    if (sink != aut->null_state())
      for (auto letter : ls.genset())
        aut->new_transition(sink, sink, letter);

    return aut;
  }

  template <typename Aut>
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
      template <typename Aut>
      automaton
      complete(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::complete(a));
      }

      REGISTER_DECLARE(complete,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_COMPLETE_HH
