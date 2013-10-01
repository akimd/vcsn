#ifndef VCSN_ALGOS_RANDOM_HH
# define VCSN_ALGOS_RANDOM_HH

# include <vcsn/labelset/nullableset.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/misc/random.hh>
# include <vcsn/misc/set.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename ctx::genset_labelset<GenSet>::letter_t
  random_label(const ctx::genset_labelset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);
    return pick(*ls.genset());
  };

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename ctx::letterset<GenSet>::label_t
  random_label(const ctx::letterset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    using super = typename ctx::letterset<GenSet>::super_type;
    return random_label(static_cast<const super&>(ls), gen);
  };

  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename ctx::nullableset<GenSet>::label_t
  random_label(const ctx::nullableset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    using super = typename ctx::nullableset<GenSet>::super_type;
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
    if (getenv("SEED"))
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
    state_set states_to_process;
    // Reachability from state[0] (_not_ from pre()).
    state_set unreachable_states;

    for (unsigned i = 0; i < num_states; ++i)
      {
        states.push_back(res.new_state());
        state_randomizer.push_back(i);
        // State 0 is "reachable" from 0.
        if (i)
          unreachable_states.emplace(i);
        if (i <= num_initial)
          res.set_initial(states[i]);
      }
    states_to_process.insert(0);

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

    while (!states_to_process.empty())
      {
        auto src = states[*states_to_process.begin()];
        std::cerr << "src: " << src << std::endl;
        states_to_process.erase(states_to_process.begin());

        // Choose a random number of successors (at least one), using
        // a binomial distribution.
        unsigned nsucc = 1 + bin(gen);

        std::cerr << "nsucc: " << nsucc << std::endl;

        // Connect to NSUCC randomly chosen successors.  We want at
        // least one unreachable successors among these if there are
        // some.
        bool saw_unreachable = false;
        auto possibilities = num_states;
        while (nsucc--)
          // No connection to unreachable successors so far.  This is
          // our last chance, so force it now.
          if (nsucc == 0
              && !saw_unreachable
              && !unreachable_states.empty())
            {
              // Pick a random unreachable state.
              std::cerr << "Pick in: ";
              print(std::cerr, unreachable_states);
              auto dst = pick.pop(unreachable_states);
              std::cerr << " => " << dst << " (unreachable: ";
              print(std::cerr, unreachable_states);
              std::cerr << ") " << std::endl;

              // Link it from src.
              std::cerr << "1: add (" << src << ", " << states[dst] << ", a)" << std::endl;
              res.add_transition(src, states[dst], random_label(*ctx.labelset(), gen));
              states_to_process.insert(dst);
              break;
            }
          else
            {
              // Pick the index of a random state.
              std::uniform_int_distribution<> dis(0, possibilities - 1);
              int index = dis(gen);
              std::cerr << "2: " << "0.." << possibilities - 1
                        << " => " << index;
              possibilities--;

              // Permute it with state_randomizer[possibilities], so
              // we cannot pick it again.
              int dst = state_randomizer[index];
              std::cerr << " (state index: " << dst << ")";
              std::swap(state_randomizer[index],
                        state_randomizer[possibilities]);

              std::cerr << " (state: " << states[dst] << ")" << std::endl;

              std::cerr << "2: add (" << src << ", " << states[dst] << ", b)" << std::endl;
              res.add_transition(src, states[dst], random_label(*ctx.labelset(), gen));

              state_set::iterator j = unreachable_states.find(dst);
              if (j != unreachable_states.end())
                {
                  std::cerr << "To process: " << states[dst] << std::endl;
                  states_to_process.insert(dst);
                  unreachable_states.erase(j);
                  saw_unreachable = true;
                }
            }

        // The state must have at least one successor.
        //	assert(!src->empty());
      }

    return std::move(res);
  }

  namespace dyn
  {
    namespace detail
    {
      /*-------------------.
      | dyn::random(aut).  |
      `-------------------*/
      /// Bridge.
      template <typename Ctx>
      automaton
      random(const context& ctx,
             unsigned num_states, float density,
             unsigned num_initial = 1, unsigned num_final = 1)
      {
        const auto& c = dynamic_cast<const Ctx&>(*ctx);
        return make_automaton(c, random<Ctx>(c, num_states, density,
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
