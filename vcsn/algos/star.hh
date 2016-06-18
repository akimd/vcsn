#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/standard.hh> // is_standard
#include <vcsn/algos/tags.hh>
#include <vcsn/core/mutable-automaton.hh>
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
  template <Automaton Aut>
  Aut&
  star_here(Aut& res, general_tag = {})
  {
    const auto& ws = *res->weightset();
    const auto& ls = *res->labelset();
    // Branch all the final states (but initials) to initial states.
    auto initial = res->new_state();
    for (auto ti: detail::make_vector(initial_transitions(res)))
      {
        res->new_transition(initial, res->dst_of(ti),
                            ls.one(), res->weight_of(ti));
        res->del_transition(ti);
      }
    for (auto tf: detail::make_vector(final_transitions(res)))
      {
        res->add_transition(res->src_of(tf), initial,
                            ls.one(), res->weight_of(tf));
        res->del_transition(tf);
      }
    res->set_initial(initial, ws.one());
    res->set_final(initial, ws.one());
    return res;
  }

  /// In-place star of a standard automaton.
  ///
  /// See standard_visitor::visit(star).
  template <Automaton Aut>
  Aut&
  star_here(Aut& res, standard_tag)
  {
    /// Standard algorithm requires standard automaton because
    /// there is a problem with "final weight of the initial state" if
    /// the automaton has several initial states. Indeed we cannot
    /// preserve several initial states since (a+b) is not equal to (a*+b*)
    require(is_standard(res), __func__, ": input must be standard");

    using automaton_t = Aut;
    using context_t = context_t_of<automaton_t>;
    using weightset_t = weightset_t_of<context_t>;
    using weight_t = weight_t_of<context_t>;
    using state_t = state_t_of<automaton_t>;

    weightset_t ws(*res->context().weightset());

    state_t initial = res->dst_of(initial_transitions(res).front());
    // The "final weight of the initial state", starred.
    weight_t w = ws.star(res->get_final_weight(initial));
    // Branch all the final states (but initial) to the successors
    // of initial.
    for (auto ti: out(res, initial))
      {
        res->lweight(ti, w);
        for (auto tf: final_transitions(res))
          if (res->src_of(tf) != initial)
            // The weight of ti has already been multiplied, on the
            // left, by w.
            res->add_transition
              (res->src_of(tf),
               res->dst_of(ti),
               res->label_of(ti),
               ws.mul(res->weight_of(tf), res->weight_of(ti)));
      }
    for (auto tf: final_transitions(res))
      res->rweight(tf, w);
    res->set_final(initial, w);
    return res;
  }

  /// In-place star of a deterministic automaton.
  template <Automaton Aut>
  Aut&
  star_here(Aut& res, deterministic_tag)
  {
    // Cannot use general star_here since it introduces spontaneous
    // transitions.  Standard star_here requires the automaton to be
    // standard.
    standard_here(res);
    star_here(res, standard_tag{});
    res = determinize(res)->strip();
    return res;
  }

  /// Star of an automaton.
  template <Automaton Aut, typename Tag = general_tag>
  auto
  star(const Aut& aut, Tag tag = {})
    -> decltype(detail::make_join_automaton(tag, aut))
  {
    auto res = detail::make_join_automaton(tag, aut);
    copy_into(aut, res);
    star_here(res, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      star(const automaton& a, const std::string& algo)
      {
        const auto& aut = a->as<Aut>();
        return ::vcsn::detail::dispatch_tags(algo,
            [aut](auto tag)
            {
              return automaton(::vcsn::star(aut, tag));
            },
        aut);
      }
    }
  }
}
