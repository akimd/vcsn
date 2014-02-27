#ifndef VCSN_ALGOS_RANDOM_HH
# define VCSN_ALGOS_RANDOM_HH

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/nullableset.hh>
# include <vcsn/misc/random.hh>
# include <vcsn/misc/set.hh>

namespace vcsn
{

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename detail::genset_labelset<GenSet>::letter_t
  random_label(const detail::genset_labelset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);
    return pick(*ls.genset());
  };

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename letterset<GenSet>::value_t
  random_label(const letterset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    using super = typename letterset<GenSet>::super_type;
    return random_label(static_cast<const super&>(ls), gen);
  };

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename nullableset<letterset<GenSet>>::value_t
  random_label(const nullableset<letterset<GenSet>>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    using super = typename nullableset<letterset<GenSet>>::super_type;
    std::uniform_int_distribution<> dis(0, 1);
    if (dis(gen))
      return ls.one();
    else
      return random_label(static_cast<const super&>(ls), gen);
  };


  /*--------------------.
  | random(automaton).  |
  `--------------------*/

  template <typename Ctx>
  mutable_automaton<Ctx>
  random(const Ctx& ctx,
         unsigned num_states, float density = 0.1,
         unsigned num_initial = 1, unsigned num_final = 1)
  {
    using automaton_t = mutable_automaton<Ctx>;
    using state_t = typename automaton_t::state_t;
    automaton_t res(ctx);

    // A good source of random integers.
    std::random_device rd;
    auto seed = rd();
    if (getenv("VCSN_SEED"))
      seed = std::mt19937::default_seed;
    std::mt19937 gen(seed);

    std::vector<state_t> states;
    states.reserve(num_states);
    // Indirect access to states[] to help random selection of successors.
    std::vector<int> state_randomizer;
    state_randomizer.reserve(num_states);

    // Using Sgi::hash_set instead of std::set for these sets is 3
    // times slower (tested on a 50000 states example).  These are
    // indexes in states[].
    using state_set = std::set<int>;
    state_set worklist;
    // Reachability from state[0] (_not_ from pre()).
    state_set unreachables;

    for (unsigned i = 0; i < num_states; ++i)
      {
        states.push_back(res.new_state());
        state_randomizer.push_back(i);
        // State 0 is "reachable" from 0.
        if (i)
          unreachables.emplace(i);
        if (i < num_initial)
          res.set_initial(states[i]);
      }
    worklist.insert(0);

    // Select the final states.
    for (unsigned i = 0; i < num_final; ++i)
      {
        std::uniform_int_distribution<> dis(i, num_states - 1);
        int index = dis(gen);
        res.set_final(states[state_randomizer[index]]);
        // Swap it at the beginning of state_randomizer, so we cannot
        // pick it again.
        std::swap(state_randomizer[index], state_randomizer[i]);
      }

    // We want to connect each state to a number of successors between
    // 1 and n.  If the probability to connect to each successor is d,
    // the number of connected successors follows a binomial distribution.
    std::binomial_distribution<> bin(num_states - 1, density);

    // Pick a member of a container following a uniform distribution.
    random_selector<std::mt19937> pick(gen);

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
                std::uniform_int_distribution<> dis(0, possibilities - 1);
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
            res.add_transition(src, states[dst],
                               random_label(*ctx.labelset(), gen));
          }
      }
    return std::move(res);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename, typename, typename, typename>
      automaton
      random(const context& ctx,
             unsigned num_states, float density,
             unsigned num_initial = 1, unsigned num_final = 1)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(random<Ctx>(c, num_states, density,
                                          num_initial, num_final));
      }

      REGISTER_DECLARE(random,
                       (const context& ctx,
                        unsigned n, float density,
                        unsigned num_initial, unsigned num_final) -> automaton);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_RANDOM_HH
