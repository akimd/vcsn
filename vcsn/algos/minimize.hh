#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <atomic>
# include <iostream>
# include <map>
# include <queue>
# include <stack>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

# include <tbb/parallel_for.h>
# include <tbb/blocked_range.h>
# include <tbb/tick_count.h>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>

namespace vcsn
{

  namespace detail
  {
    template <typename Aut>
    class minimizer
    {
      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;

      static_assert(automaton_t::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(std::is_same<typename automaton_t::weight_t, bool>::value,
                    "requires Boolean weights");

    public:
      Aut operator()(const Aut& a)
      {
        if (!is_deterministic(a))
          throw std::domain_error("minimize: requires a deterministic automaton");

        const auto& letters = *a.labelset();
        /*
        ** We use the Moore's algorithm.
        ** We try to partition our automaton into equivalency classes.
        ** To do so, we first cut our automaton into two obvious classes:
        ** final states and the other. Then, for every state, for every letter, we
        ** distinguish those which goes into the same equivalency class, and others
        ** Repeat this process until no partition is done.
        */

        // Init. Eq[0] = finals, Eq[1] = others.
        equivalences.reserve(a.num_all_states());
        for (auto st : a.states())
          equivalences[st] = (a.is_final(st) ? 1 : 2);

        tbb::tick_count before = tbb::tick_count::now();
        if (!parallel)
          {
            //std::cerr << "NO PARALLEL\n";
            do
              {
                for (auto letter : letters)
                  {
                    labels.clear();
                    for (auto st : a.states())
                      {
                        // Here we test for each letter start and end equivalence
                        // class, then label the state
                        int b1 = equivalences[st];

                        int b2 = -1;
                        for (auto tr : a.out(st))
                          if (a.label_of(tr) == letter)
                            {
                              b2 = equivalences[a.dst_of(tr)];
                              break;
                            }

                        if (b2 != -1)
                          {
                            labels[std::make_pair(b1, b2)].push_back(st);
                          }
                      }

                    nbr_eq_classes_last_loop = eq_class;
                    eq_class = 2;
                    for (auto& label : labels) // foreach state having the same label
                      {
                        for (auto st : label.second)
                          equivalences[st] = eq_class;
                        ++eq_class;
                      }
                  }
              }
            while (nbr_eq_classes_last_loop != eq_class);
          }
        else
          {
            std::cerr << "PARALLEL\n";
            int nb_states = a.states().size();
            state_t tmp_states[nb_states];
            std::copy(a.states().begin(), a.states().end(), tmp_states);
            std::pair<int, int> result[a.num_all_states()];

            do
              {
                for (auto letter : letters)
                  {
                    std::map<std::pair<int, int>, int> pair_to_eq;
                    tbb::parallel_for(size_t(0), size_t(nb_states),
                                      [this, &a, &tmp_states, &result, letter] (size_t idx)
                                      {
                                        state_t st = tmp_states[idx];
                                        // Here we test for each letter start and end equivalence
                                        // class, then label the state
                                        int start_c = equivalences[st];

                                        int end_c = -1;
                                        for (auto tr : a.out(st))
                                          if (a.label_of(tr) == letter)
                                            {
                                              end_c = equivalences[a.dst_of(tr)];
                                              break;
                                            }

                                        if (end_c != -1)
                                          result[st] = std::make_pair(start_c, end_c);
                                      });

                    nbr_eq_classes_last_loop = eq_class;
                    eq_class = 2;
                    int res_size = a.num_all_states();
                    for (int i = 2; i < res_size; ++i)
                      {
                        auto src_dst = result[i];
                        auto exists = pair_to_eq.find(src_dst);
                        if (exists == pair_to_eq.end())
                          {
                            pair_to_eq[src_dst] = eq_class;
                            ++eq_class;
                          }
                      }

                    tbb::parallel_for (2, res_size,
                                       [this, &result, &pair_to_eq] (int i)
                                       {
                                         auto src_dst = result[i];
                                         equivalences[i] = pair_to_eq[src_dst];
                                       });
                  }
              }
            while (nbr_eq_classes_last_loop != eq_class);
          }

        std::map<int, state_t> eq_to_res;
        equivalences[a.pre()] = 0;
        equivalences[a.post()] = 1;
        automaton_t res{a.context()};
        eq_to_res[0] = res.pre();
        eq_to_res[1] = res.post();

        std::queue<state_t> todo;
        todo.push(a.pre());

        //Work in progress
        while (!todo.empty())
          {
            const state_t asrc = todo.front();
            todo.pop();

            for (auto tr : a.all_out(asrc))
              {
                state_t dst = a.dst_of(tr);
                auto p = eq_to_res.emplace(equivalences[dst], a.null_state());
                if (p.second)
                  {
                    todo.push(dst);
                    p.first->second = res.new_state();
                  }

                res.add_transition(eq_to_res[equivalences[asrc]], p.first->second,
                                   a.label_of(tr), a.weight_of(tr));
              }
          }
        tbb::tick_count after = tbb::tick_count::now();
        std::cerr << "time: " << (after - before).seconds() << std::endl;

        return res;
      }

      std::vector<int> equivalences;
      std::map<std::pair<int, int>, std::vector<state_t>> labels;

      bool parallel = getenv("VCSN_PARALLEL");
      
      int eq_class = 2;
      int nbr_eq_classes_last_loop = 2;
    };
  }

  template <typename Aut>
  inline
  Aut
  minimize(const Aut& a)
  {
    detail::minimizer<Aut> minimize;
    return minimize(a);
  }

  /*----------------.
    | dyn::minimize.  |
    `----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut>
      automaton
      minimize(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(minimize(a));
      }

      REGISTER_DECLARE(minimize, (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_HH
