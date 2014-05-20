#ifndef VCSN_CORE_AUTOMATONSET_HH
# define VCSN_CORE_AUTOMATONSET_HH

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/ctx/context.hh>

namespace vcsn
{

  template <typename Context>
  class automatonset
  {
  public:
    using context_t = Context;
    using value_t = mutable_automaton<context_t>;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using kind_t = typename context_t::kind_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;

    /// Lightweight state handle (or index).
    using state_t = unsigned;
    /// Lightweight transition handle (or index).
    using transition_t = unsigned;
    /// Transition label.
    using label_t = typename labelset_t::value_t;
    /// Transition weight.
    using weight_t = typename weightset_t::value_t;

  protected:
    context_t ctx_;
  };
}

#endif // !VCSN_CORE_AUTOMATONSET_HH
