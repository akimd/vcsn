#ifndef VCSN_ALGOS_RANDOM_HH
# define VCSN_ALGOS_RANDOM_HH

# include <vcsn/misc/random.hh>
# include <vcsn/misc/set.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{

  /*--------------------.
  | random(automaton).  |
  `--------------------*/

  template <typename Ctx>
  mutable_automaton<Ctx>
  random(const Ctx& ctx,
         unsigned num_states, float density)
  {
    using automaton_t = mutable_automaton<Ctx>;
    using state_t = typename automaton_t::state_t;
    automaton_t res(ctx);

    std::cerr << "density: " << density << std::endl;

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
    state_set unreachable_states;

    for (unsigned i = 0; i < num_states; ++i)
      {
        states.push_back(res.new_state());
        state_randomizer.push_back(i);
        unreachable_states.emplace(i);
      }
    std::cerr << "state_randomizer:";
    for (auto i: state_randomizer)
      std::cerr << ' ' << i;
    std::cerr << std::endl;

    states_to_process.insert(0);

    // We want to connect each state to a number of successors between
    // 1 and n.  If the probability to connect to each successor is d,
    // the number of connected successors follows a binomial distribution.
    std::random_device rd;
    std::mt19937 gen(rd());
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
              res.add_transition(src, states[dst], 'a');
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
              res.add_transition(src, states[dst], 'b');

              state_set::iterator j = unreachable_states.find(dst);
              if (j != unreachable_states.end())
                {
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
      random(const context& ctx, unsigned num_states, float density)
      {
        const auto& c = dynamic_cast<const Ctx&>(*ctx);
        return make_automaton(c, random<Ctx>(c, num_states, density));
      }

      REGISTER_DECLARE(random,
                       (const context& ctx,
                        unsigned n, float density) -> automaton);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_RANDOM_HH
