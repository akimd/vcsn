#ifndef VCSN_ALGOS_COPY_HH
# define VCSN_ALGOS_COPY_HH

# include <unordered_map>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/fwd.hh>

namespace vcsn
{
  /// Copy an automaton.
  template <typename AutIn, typename AutOut>
  void
  copy(const AutIn& in, AutOut& out)
  {
    using in_state_t = typename AutIn::state_t;
    using out_state_t = typename AutOut::state_t;

    std::unordered_map<in_state_t, out_state_t> out_state;

    for (auto s : in.states())
    {
      out_state_t ns =  out.new_state();
      out_state[s] = ns;
      out.set_initial(ns, in.get_initial_weight(s));
      out.set_final(ns, in.get_final_weight(s));
    }
    for (auto t : in.transitions())
      out.set_transition(out_state[in.src_of(t)],
                         out_state[in.dst_of(t)],
                         in.label_of(t), in.weight_of(t));
  }

  template <typename Aut>
  Aut
  copy(const Aut& input)
  {
    using automaton_t = Aut;
    automaton_t output{input.context()};
    copy(input,output);
    return output;
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
        return make_automaton(a.context(), copy(a));
      }

      REGISTER_DECLARE(copy,
                       (const automaton&) -> automaton);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_COPY_HH
