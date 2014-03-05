#ifndef VCSN_ALGOS_LEFT_MULT_HH
# define VCSN_ALGOS_LEFT_MULT_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/weight.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  namespace detail
  {
    template <typename Aut>
    struct standard_operations
    {
      using automaton_t = Aut;
      using context_t = typename automaton_t::context_t;
      using weight_t = typename context_t::weight_t;
      using weightset_t = typename context_t::weightset_t;
      using state_t = typename automaton_t::state_t;

      static automaton_t&
      left_mult_here(automaton_t& res, const weight_t& w)
      {
        require(is_standard(res), __func__, ": input must be standard");

        weightset_t ws(*res.context().weightset());
        state_t initial = res.dst_of(res.initial_transitions().front());

        if (!ws.is_one(w))
          for (auto t: res.all_out(initial))
            res.lmul_weight(t, w);
        return res;
      }

      static automaton_t&
      right_mult_here(automaton_t& res, const weight_t& w)
      {
        require(is_standard(res), __func__, ": input must be standard");
        for (auto t: res.final_transitions())
          res.rmul_weight(t, w);
        return res;
      }
    };
  }

  /*-----------------------.
  | left-mult(automaton).  |
  `-----------------------*/

  template <typename Aut>
  Aut&
  left_mult_here(Aut& res, const typename Aut::context_t::weight_t& w)
  {
    return detail::standard_operations<Aut>::left_mult_here(res, w);
  }

  template <class Aut>
  Aut
  left_mult(const Aut& aut, const typename Aut::context_t::weight_t& w)
  {
    auto res = copy(aut);
    left_mult_here(res, w);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename WeightSet>
      automaton
      left_mult(const automaton& aut, const weight& weight)
      {
        const auto& a = aut->as<Aut>();
        const auto& w = weight->as<WeightSet>().weight();
        return make_automaton(left_mult(a, w));
      }

      REGISTER_DECLARE(left_mult,
                       (const automaton&, const weight&) -> automaton);

    }
  }

  /*------------------------.
  | right-mult(automaton).  |
  `------------------------*/

  template <typename Aut>
  Aut&
  right_mult_here(Aut& res, const typename Aut::context_t::weight_t& w)
  {
    return detail::standard_operations<Aut>::right_mult_here(res, w);
  }

  template <class Aut>
  Aut
  right_mult(const Aut& aut, const typename Aut::context_t::weight_t& w)
  {
    auto res = copy(aut);
    right_mult_here(res, w);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename WeightSet>
      automaton
      right_mult(const automaton& aut, const weight& weight)
      {
        const auto& a = aut->as<Aut>();
        const auto& w = weight->as<WeightSet>().weight();
        return make_automaton(right_mult(a, w));
      }

      REGISTER_DECLARE(right_mult,
                       (const automaton&, const weight&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_LEFT_MULT_HH
