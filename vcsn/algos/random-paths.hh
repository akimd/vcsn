#include <algorithm>

#include <vcsn/algos/random.hh>
#include <vcsn/core/automaton.hh>
#include <vcsn/misc/random.hh>
#include <vcsn/misc/unordered_set.hh>

namespace vcsn
{

  /// Randomly selects a transition using the uniform distribution
  template <Automaton Aut,
            typename RandomGenerator = std::default_random_engine>
  class uniform_path_selector
  {
  public:
    using automaton_t = std::remove_cv_t<Aut>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    uniform_path_selector(RandomGenerator& gen = RandomGenerator())
      : gen_(gen)
      , pick_(gen)
    {}

    template <typename Container>
    transition_t operator()(const automaton_t&, const Container& trs)
    {
      return pick_(trs);
    }

  private:
    RandomGenerator gen_;
    random_selector<RandomGenerator> pick_;
  };

  /// Randomly selects a transition using weight probabilites after normalizing
  /// on the sum of the output weights
  template <Automaton Aut,
            typename RandomGenerator = std::default_random_engine>
  class prob_path_selector
  {
  public:
    using automaton_t = std::remove_cv_t<Aut>;
    using weightset_t = weightset_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;

    prob_path_selector(RandomGenerator& gen = RandomGenerator())
      : gen_(gen)
    {}

    template <typename Container>
    transition_t operator()(const automaton_t& aut, const Container& trs)
    {
      weight_t sum = 0;
      for (auto tr : trs)
        sum += aut->weight_of(tr); // TODO: generic with weightset!

      std::uniform_int_distribution<int> distr(0, sum - 1);
      unsigned rnd = distr(gen_);

      for (auto tr : trs)
        {
          auto w = aut->weight_of(tr);
          if (rnd < w)
            return tr;
          rnd -= w;
        }

      return {}; // Should never happen
    }

  private:
    RandomGenerator gen_;
  };


  template <Automaton Aut,
            typename PathSelector>
  class random_pathfinder
  {
  public:
    using automaton_t = std::remove_cv_t<Aut>;
    using weightset_t = weightset_t_of<automaton_t>;
    using labelset_t = labelset_t_of<automaton_t>;

    using transition_t = transition_t_of<automaton_t>;
    using label_t = label_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;

    /// Initialize the random pathfinder. This algorithm uses a path selector
    /// to determine what transition to select.
    random_pathfinder(const automaton_t& aut, unsigned num_paths,
                      PathSelector& selector)
      : in_(aut)
      , out_(make_shared_ptr<automaton_t>(aut->context()))
      , selector_(selector)
      , num_paths_(num_paths)
    {
    }

    /// Generate a set of paths in the input automaton and returns the
    /// resulting, "sampled" automaton.
    automaton_t operator()()
    {
      for (unsigned i = 0; i < num_paths_; i++)
        {
          visited_.clear();
          explore(in_->pre(), out_->pre());
        }
      return out_;
    }

  private:
    void explore(state_t in_src, state_t out_src)
    {
      visited_.insert(in_src);

      if (in_src == in_->post())
        return;

      std::vector<transition_t> in_dests;
      auto trs = all_out(in_, in_src);
      std::copy_if(std::begin(trs), std::end(trs),
                   std::back_inserter(in_dests),
                   [&](transition_t t) {
                     return !has(visited_, in_->dst_of(t));
                   });

      if (!in_dests.empty())
        {
          transition_t tr = selector_(in_, in_dests);
          state_t in_dst = in_->dst_of(tr);
          state_t out_dst;

          if (in_dst == in_->post())
            out_dst = out_->post();
          else
            out_dst = out_->new_state();

          out_->add_transition(out_src, out_dst, in_->label_of(tr),
                               in_->weight_of(tr));
          explore(in_dst, out_dst);
        }
    }

    automaton_t in_;
    automaton_t out_;
    PathSelector selector_;
    unsigned num_paths_;

    std::unordered_set<state_t> visited_;
  };

  /// Wrapper function to generate a set of paths in the input automaton and
  /// returns the resulting, "sampled" automaton using an uniform distribution.
  template <Automaton Aut>
  Aut random_paths(Aut aut, unsigned num_paths)
  {
    // TODO: take a selector argument once they work in a generic way.

    using automaton_t = Aut;
    using selector_t = uniform_path_selector<automaton_t, std::mt19937>;
    using pathfinder_t = random_pathfinder<automaton_t, selector_t>;

    std::random_device rd;
    auto seed = rd();
    if (getenv("VCSN_SEED"))
      seed = std::mt19937::default_seed;
    std::mt19937 gen(seed);

    selector_t selector(gen);
    pathfinder_t pathfinder(aut, num_paths, selector);
    return pathfinder();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (random_paths).
      template <Automaton Aut, typename Integer>
      automaton random_paths(const automaton& aut, unsigned num_paths)
      {
        // TODO: allow the user to pass a selector string once the selectors
        // work in a generic way.

        const auto& a = aut->as<Aut>();
        return make_automaton(random_paths(a, num_paths));
      }
    }
  }
} // end namespace vcsn
