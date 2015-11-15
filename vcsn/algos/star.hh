#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/standard.hh> // is_standard
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/misc/raise.hh> // require
#include <vcsn/misc/vector.hh>

namespace vcsn
{
  /*------.
  | star  |
  `------*/

  /// In-place star of an automaton.
  template <typename Aut>
  Aut&
  star_here(Aut& res, general_tag = {})
  {
    const auto& ws = *res->weightset();
    const auto& ls = *res->labelset();
    // Branch all the final states (but initials) to initial states.
    auto initial = res->new_state();
    for (auto ti: detail::make_vector(res->initial_transitions()))
      {
        res->new_transition(initial, res->dst_of(ti), ls.one(), res->weight_of(ti));
        res->del_transition(ti);
      }
    for (auto tf: res->final_transitions())
      res->add_transition(res->src_of(tf), initial, ls.one(), res->weight_of(tf));
    res->set_initial(initial, ws.one());
    res->set_final(initial, ws.one());
    return res;
  }


  /// In-place star of a standard automaton.
  ///
  /// See standard_visitor::visit(star).
  template <typename Aut>
  Aut&
  star_here(Aut& res, standard_tag)
  {
    require(is_standard(res), __func__, ": input must be standard");

    using automaton_t = Aut;
    using context_t = context_t_of<automaton_t>;
    using weightset_t = weightset_t_of<context_t>;
    using weight_t = weight_t_of<context_t>;
    using state_t = state_t_of<automaton_t>;

    weightset_t ws(*res->context().weightset());

    state_t initial = res->dst_of(res->initial_transitions().front());
    // The "final weight of the initial state", starred.
    weight_t w = ws.star(res->get_final_weight(initial));
    // Branch all the final states (but initial) to the successors
    // of initial.
    for (auto ti: res->out(initial))
      {
        res->lmul_weight(ti, w);
        for (auto tf: res->final_transitions())
          if (res->src_of(tf) != initial)
            // The weight of ti has already been multiplied, on the
            // left, by w.
            res->add_transition
              (res->src_of(tf),
               res->dst_of(ti),
               res->label_of(ti),
               ws.mul(res->weight_of(tf), res->weight_of(ti)));
      }
    for (auto tf: res->final_transitions())
      res->rmul_weight(tf, w);
    res->set_final(initial, w);
    return res;
  }

  /// Star of an automaton.
  template <typename Aut, typename Tag = general_tag>
  auto
  star(const Aut& aut, Tag tag = {})
    -> decltype(make_tag_automaton(aut, tag))
  {
    auto res = make_tag_automaton(aut, tag);
    copy_into(aut, res);
    star_here(res, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      automaton
      star(const automaton& a, const std::string& algo)
      {
        const auto& aut = a->as<Aut>();
        return ::vcsn::detail::dispatch_standard(algo,
            [aut](auto tag)
            {
              return make_automaton(::vcsn::star(aut, tag));
            },
        aut);
      }
    }
  }
}
