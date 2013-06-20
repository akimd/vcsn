#ifndef VCSN_ALGOS_COPY_HH
# define VCSN_ALGOS_COPY_HH

# include <unordered_map>

# include <vcsn/misc/set.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/fwd.hh>

namespace vcsn
{
  /// Copy an automaton.
  template <typename AutIn, typename AutOut>
  void
  copy(const AutIn& in, AutOut& out,
       std::function<bool(typename AutIn::state_t)> keep_state)
  {
    using in_state_t = typename AutIn::state_t;
    using out_state_t = typename AutOut::state_t;

    std::unordered_map<in_state_t, out_state_t> out_state;

    for (auto s : in.states())
      if (keep_state(s))
        {
          out_state_t ns =  out.new_state();
          out_state[s] = ns;
          out.set_initial(ns, in.get_initial_weight(s));
          out.set_final(ns, in.get_final_weight(s));
        }
    for (auto t : in.transitions())
      if (keep_state(in.src_of(t))
          && keep_state(in.dst_of(t)))
        out.set_transition(out_state[in.src_of(t)],
                           out_state[in.dst_of(t)],
                           in.label_of(t),
                           in.weight_of(t));
  }

  template <typename State>
  bool
  keep_all_states(State)
  {
    return true;
  }

  /// A copy of \a input keeping only its states that are accepted by
  /// \a keep_state.
  template <typename Aut>
  Aut
  copy(const Aut& input,
       std::function<bool(typename Aut::state_t)> keep_state)
  {
    using automaton_t = Aut;
    automaton_t output{input.context()};
    /// Beware of clashes with std::copy.
    ::vcsn::copy(input, output, keep_state);
    return output;
  }

  /// Convenience wrapper for lambdas for instance.
  template <typename Aut, typename StatePred>
  Aut
  copy(const Aut& input, StatePred keep_state)
  {
    return::vcsn::copy(input,
                       std::function<bool(typename Aut::state_t)>{keep_state});
  }

  /// A copy of \a input keeping only its states that are members of
  /// \a keep.
  template <typename Aut>
  Aut
  copy(const Aut& input, const std::set<typename Aut::state_t>& keep)
  {
    using state_t = typename Aut::state_t;
    return ::vcsn::copy(input, [&keep](state_t s)
                        {
                          return has(keep, s);
                        });
  }

  /// Clone \a input.
  // FIXME: Is there a means to use default arguments?
  template <typename Aut>
  Aut
  copy(const Aut& input)
  {
    return ::vcsn::copy(input, {keep_all_states<typename Aut::state_t>});
  }

  /*-----------.
  | dyn::copy. |
  `-----------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      copy(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), ::vcsn::copy(a));
      }

      REGISTER_DECLARE(copy,
                       (const automaton&) -> automaton);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_COPY_HH
