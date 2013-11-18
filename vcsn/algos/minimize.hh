#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <atomic>
# include <iostream>
# include <map>
# include <queue>
# include <set>
# include <string>
# include <type_traits>
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
      minimizer(const Aut& a)
        : aut_(a)
      {
        if (!is_deterministic(a))
          throw std::domain_error("minimize: requires a deterministic automaton");
      }

      std::map<int, state_t> eq_to_res;

      Aut operator()()
      {
        /*
        ** We use the Moore's algorithm.
        ** We try to partition our automaton into equivalency classes.
        ** To do so, we first cut our automaton into two obvious classes:
        ** final states and the other. Then, for every state, for every letter, we
        ** distinguish those which goes into the same equivalency class, and others
        ** Repeat this process until no partition is done.
        */

        // Init. Eq[0] = finals, Eq[1] = others.
        equivalences.reserve(aut_.num_all_states());
        for (auto st : aut_.states())
          equivalences[st] = (aut_.is_final(st) ? 1 : 2);

        tbb::tick_count before = tbb::tick_count::now();
        if (getenv("VCSN_PARALLEL"))
          {
            std::cerr << "PARALLEL\n";
            compute_classes_parallel_();
          }
        else
          {
            //std::cerr << "NO PARALLEL\n";
            compute_classes_serial_();
          }

        equivalences[aut_.pre()] = 0;
        equivalences[aut_.post()] = 1;
        automaton_t res{aut_.context()};
        eq_to_res[0] = res.pre();
        eq_to_res[1] = res.post();

        std::queue<state_t> todo;
        todo.push(aut_.pre());

        //Work in progress
        while (!todo.empty())
          {
            const state_t asrc = todo.front();
            todo.pop();

            for (auto tr : aut_.all_out(asrc))
              {
                state_t dst = aut_.dst_of(tr);
                auto p = eq_to_res.emplace(equivalences[dst], aut_.null_state());
                if (p.second)
                  {
                    todo.push(dst);
                    p.first->second = res.new_state();
                  }
                res.add_transition(eq_to_res[equivalences[asrc]],
                                   p.first->second,
                                   aut_.label_of(tr), aut_.weight_of(tr));
              }
          }
        tbb::tick_count after = tbb::tick_count::now();
        std::cerr << "time: " << (after - before).seconds() << std::endl;

        return res;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      origins_t
      origins()
      {
        origins_t res;
        for (auto s: aut_.states())
          res[eq_to_res[equivalences[s]]].insert(s);
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n";
        for (auto p : orig)
          {
            o << "    " << p.first - 2
              << " [label = \"";
            const char* sep = "";
            for (auto s: p.second)
              {
                o << sep << s - 2;
                sep = ",";
              }
            o << "\"]\n";
          }
        o << "*/\n";
        return o;
      }

    private:
      void compute_classes_serial_orig_()
      {
        const auto& letters = *aut_.labelset();
        do
          {
            for (auto letter : letters)
              {
                labels.clear();
                for (auto st : aut_.states())
                  {
                    // Here we test for each letter start and end equivalence
                    // class, then label the state
                    int b1 = equivalences[st];

                    int b2 = -1;
                    for (auto tr : aut_.out(st))
                      if (aut_.label_of(tr) == letter)
                        {
                          b2 = equivalences[aut_.dst_of(tr)];
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

      void compute_classes_parallel_()
      {
        const auto& letters = *aut_.labelset();
        int nb_states = aut_.states().size();
        state_t tmp_states[nb_states];
        std::copy(aut_.states().begin(), aut_.states().end(), tmp_states);
        std::pair<int, int> result[aut_.num_all_states()];

        do
          {
            for (auto letter : letters)
              {
                std::map<std::pair<int, int>, int> pair_to_eq;
                tbb::parallel_for
                  (size_t(0), size_t(nb_states),
                   [this, &tmp_states, &result, letter] (size_t idx)
                   {
                     state_t st = tmp_states[idx];
                     // Here we test for each letter start and end equivalence
                     // class, then label the state
                     int start_c = equivalences[st];

                     int end_c = -1;
                     for (auto tr : aut_.out(st))
                       if (aut_.label_of(tr) == letter)
                         {
                           end_c = equivalences[aut_.dst_of(tr)];
                           break;
                         }

                     if (end_c != -1)
                       result[st] = std::make_pair(start_c, end_c);
                   });

                nbr_eq_classes_last_loop = eq_class;
                eq_class = 2;
                int res_size = aut_.num_all_states();
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

                tbb::parallel_for
                  (2, res_size,
                   [this, &result, &pair_to_eq] (int i)
                   {
                     auto src_dst = result[i];
                     equivalences[i] = pair_to_eq[src_dst];
                   });
              }
          }
        while (nbr_eq_classes_last_loop != eq_class);
      }

      void compute_classes_serial_()
      {
        const auto& letters = *aut_.labelset();
        std::pair<int, int> result[aut_.num_all_states()];

        do
          {
            for (auto letter : letters)
              {
                for (auto st: aut_.states())
                  {
                    // Here we test for each letter start and end equivalence
                    // class, then label the state
                    int start_c = equivalences[st];

                    int end_c = -1;
                    for (auto tr : aut_.out(st))
                      if (aut_.label_of(tr) == letter)
                        {
                          end_c = equivalences[aut_.dst_of(tr)];
                          break;
                        }

                    if (end_c != -1)
                      result[st] = std::make_pair(start_c, end_c);
                  }

                nbr_eq_classes_last_loop = eq_class;
                eq_class = 2;
                std::map<std::pair<int, int>, int> pair_to_eq;
                for (auto i : aut_.states())
                  {
                    auto src_dst = result[i];
                    if (pair_to_eq.find(src_dst) == pair_to_eq.end())
                      {
                        pair_to_eq[src_dst] = eq_class;
                        ++eq_class;
                      }
                  }

                for (auto i : aut_.states())
                  {
                    auto src_dst = result[i];
                    equivalences[i] = pair_to_eq[src_dst];
                  }
              }
          }
        while (nbr_eq_classes_last_loop != eq_class);
      }

      std::vector<int> equivalences;
      std::map<std::pair<int, int>, std::vector<state_t>> labels;
      const Aut& aut_;
      int eq_class = 2;
      int nbr_eq_classes_last_loop = 2;
    };
  }

  template <typename Aut>
  inline
  Aut
  minimize(const Aut& a)
  {
    detail::minimizer<Aut> minimize(a);
    auto res = minimize();
    if (getenv("VCSN_ORIGINS"))
      minimize.print(std::cout, minimize.origins());
    return res;
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
