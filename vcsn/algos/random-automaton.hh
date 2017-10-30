#pragma once

#include <vcsn/algos/random-weight.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/irange.hh>

namespace vcsn
{
  /*--------------------.
  | random_automaton.   |
  `--------------------*/

  /// Produce a random automaton.
  ///
  /// \param ctx
  ///    The context of the result.
  /// \param num_states
  ///    The number of states wanted in the automata
  ///    (>0).  All states will be connected, and there will be no dead
  ///    state.  However, some might not be coaccessible.
  /// \param density
  ///    The density of the automata.  This is the probability
  ///    (between 0.0 and 1.0), to add a transition between two
  ///    states.  All states have at least one outgoing transition, so
  ///    \a d is considered only when adding the remaining transition.
  ///    A density of 1 means all states will be connected to each
  ///    other.
  /// \param num_initial
  ///    The number of initial states wanted (0 <= num_initial <= num_states)
  /// \param num_final
  ///    The number of final states wanted (0 <= num_final <= num_states)
  /// \param max_labels
  ///    The maximum number of labels per transition.  Defaults to the
  ///    number of generators.
  /// \param loop_chance
  ///    The probability (between 0.0 and 1.0) for each state to have
  ///    a loop.
  /// \param weights
  ///    The specification string (following the format of random_weight) used
  ///    to generate weights on each transitions.
  template <typename Ctx>
  mutable_automaton<Ctx>
  random_automaton(const Ctx& ctx,
                   unsigned num_states, float density = 0.1,
                   unsigned num_initial = 1, unsigned num_final = 1,
                   boost::optional<unsigned> max_labels = {},
                   float loop_chance = 0.0, const std::string& weights = "")
  {
    require(0 <= density && density <= 1,
            "random_automaton: density must be in [0,1]");
    require(0 <= loop_chance && loop_chance <= 1,
            "random_automaton: loop chance must be in [0,1]");

    using detail::irange;
    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    auto res = make_shared_ptr<automaton_t>(ctx);

    auto& gen = make_random_engine();

    const auto& ws = *ctx.weightset();

    // Labels.
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();
    auto num_gens = boost::distance(gens) + ls.has_one();
    require(num_gens,
            "random_automaton: empty labelset: ", ls);
    if (max_labels)
      {
        require(0 < *max_labels,
                "random_automaton: max number of labels cannot be null");

        require(*max_labels <= num_gens,
                "random_automaton: max number of labels cannot be greater "
                "than the number of generators");
      }
    else
      max_labels = num_gens;
    auto num_labels = std::uniform_int_distribution<>(1, *max_labels);

    auto random_label = [&ls, &gen]()
    {
      return vcsn::random_label(ls, "", gen);
    };

    auto random_weight = [&ws, &weights, &gen]()
    {
      return weights.empty() ? ws.one() : vcsn::random_weight(ws, weights, gen);
    };

    auto states = std::vector<state_t>{};
    states.reserve(num_states);
    // Indirect access to states[] to help random selection of successors.
    auto state_randomizer = std::vector<int>{};
    state_randomizer.reserve(num_states);

    // Using Sgi::hash_set instead of std::set for these sets is 3
    // times slower (tested on a 50000 states example).  These are
    // indexes in states[].
    using state_set = std::set<int>;
    state_set worklist;
    // Reachability from state[0] (_not_ from pre()).
    state_set unreachables;

    for (unsigned i: detail::irange(num_states))
      {
        states.emplace_back(res->new_state());
        state_randomizer.emplace_back(i);
        // State 0 is "reachable" from 0.
        if (i)
          unreachables.emplace(i);
        if (i < num_initial)
          res->set_initial(states[i]);
      }
    worklist.insert(0);

    // Select the final states.
    for (unsigned i: detail::irange(num_final))
      {
        auto dis = std::uniform_int_distribution<>(i, num_states - 1);
        int index = dis(gen);
        res->set_final(states[state_randomizer[index]]);
        // Swap it at the beginning of state_randomizer, so we cannot
        // pick it again.
        std::swap(state_randomizer[index], state_randomizer[i]);
      }

    // We want to connect each state to a number of successors between
    // 1 and n.  If the probability to connect to each successor is d,
    // the number of connected successors follows a binomial distribution.
    auto bin = std::binomial_distribution<>(num_states - 1, density);

    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);

    while (!worklist.empty())
      {
        auto src = states[*worklist.begin()];
        worklist.erase(worklist.begin());

        // Choose a random number of successors (at least one), using
        // a binomial distribution.
        unsigned nsucc = 1 + bin(gen);

        // Connect to NSUCC randomly chosen successors.  We want at
        // least one unreachable successors among these if there are
        // some.
        bool saw_unreachable = false;
        auto possibilities = num_states;
        while (nsucc--)
          {
            // The index (in states[]) of the destination.
            unsigned dst = -1;
            // No connection to unreachable successors so far.  This is
            // our last chance, so force it now.
            if (nsucc == 0
                && !saw_unreachable
                && !unreachables.empty())
              {
                // Pick a random unreachable state.
                dst = pick.pop(unreachables);
                worklist.insert(dst);
              }
            else
              {
                // Pick the index of a random state.
                auto dis
                  = std::uniform_int_distribution<>(0, possibilities - 1);
                int index = dis(gen);
                possibilities--;

                dst = state_randomizer[index];

                // Permute it with state_randomizer[possibilities], so
                // we cannot pick it again.
                std::swap(state_randomizer[index],
                          state_randomizer[possibilities]);

                state_set::iterator j = unreachables.find(dst);
                if (j != unreachables.end())
                  {
                    worklist.insert(dst);
                    unreachables.erase(j);
                    saw_unreachable = true;
                  }
              }
            auto n = num_labels(gen);
            for (auto _: detail::irange(n))
              res->add_transition(src, states[dst],
                                  random_label(), random_weight());
          }
      }

    // Add loops.
    if (0 < loop_chance)
      {
        auto dis = std::bernoulli_distribution(loop_chance);
        for (auto s : res->states())
          if (dis(gen))
            res->add_transition(s, s,
                                random_label(), random_weight());

      }

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename NumStates, typename Density,
                typename NumInitial, typename NumFinal,
                typename MaxLabels, typename LoopChance, typename String>
      automaton
      random_automaton(const context& ctx,
                       unsigned num_states, float density,
                       unsigned num_initial, unsigned num_final,
                       boost::optional<unsigned> max_labels,
                       float loop_chance,
                       const std::string& weights)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::random_automaton(c, num_states, density,
                                      num_initial, num_final,
                                      max_labels,
                                      loop_chance,
                                      weights);
      }
    }
  }


  /*----------------------------------.
  | random_automaton_deterministic.   |
  `----------------------------------*/

  template <typename Ctx>
  mutable_automaton<Ctx>
  random_automaton_deterministic(const Ctx& ctx, unsigned num_states)
  {
    require(0 < num_states, "num_states must be > 0");

    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    auto& gen = make_random_engine();
    auto dis = std::uniform_int_distribution<int>(0, num_states - 1);

    auto states = std::vector<state_t>{};
    states.reserve(num_states);

    for (auto _: detail::irange(num_states))
      states.emplace_back(res->new_state());

    for (auto i: detail::irange(num_states))
      for (auto l : ctx.labelset()->generators())
        res->add_transition(states[i], states[dis(gen)], l,
                            ctx.weightset()->one());

    res->set_initial(states[dis(gen)]);
    res->set_final(states[dis(gen)]);

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <typename Ctx, typename>
      automaton
      random_automaton_deterministic(const context& ctx, unsigned num_states)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::random_automaton_deterministic(c, num_states);
      }
    }
  }
}
