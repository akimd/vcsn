#ifndef VCSN_ALGOS_COPY_HH
# define VCSN_ALGOS_COPY_HH

# include <unordered_map>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/fwd.hh>

namespace vcsn
{
  /* Function that copies an automaton.
     FIXME
     Shouldn't be a clone() method more suitable ?
     */
  template <typename Aut, typename Aut2>
  void
  copy(const Aut& input, Aut2& output)
  {
    using in_state_t = typename Aut::state_t;
    using out_state_t = typename Aut2::state_t;

    std::unordered_map<in_state_t, out_state_t> output_state;

    for (auto s : input.states())
    {
      out_state_t ns =  output.new_state();
      output_state[s] = ns;
      output.set_initial(ns, input.get_initial_weight(s));
      output.set_final(ns, input.get_final_weight(s));
    }
    for (auto t : input.transitions())
      output.set_transition(output_state[input.src_of(t)],
          output_state[input.dst_of(t)],
          input.label_of(t), input.weight_of(t));
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
