#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <iostream>
# include <map>
# include <stack>
# include <string>
# include <type_traits>
# include <thread>
# include <unordered_map>
# include <vector>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>

namespace vcsn
{
  template <class Aut>
  inline
  Aut
  minimize(const Aut& a)
  {
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean weights");

    //using automaton_t = Aut;
    //using label_t = typename automaton_t::label_t;
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;

    const auto& letters = *a.labelset();
    automaton_t res{a.context()};

    /*
    ** We use the Moore's algorithm.
    ** We try to partition our automaton into equivalency classes.
    ** To do so, we first cut our automaton into two obvious classes:
    ** final states and the other. Then, for every state, for every letter, we
    ** distinguish those which goes into the same equivalency class, and others
    ** Repeat this process until no partition is done.
    */

    // Set of states.
    using state_set = dynamic_bitset;

    std::map<state_t, int> equivalences;

    const unsigned int cores = std::thread::hardware_concurrency();

    // Init. Eq[0] = finals, Eq[1] = others.

    for (auto st : a.states())
        equivalences[st] = (a.is_final(st) ? 1 : 2);

    std::map<std::pair<int, int>, std::vector<state_t>> labels;

    int eq_class = 1;
    int nbr_eq_classes_last_loop = 2;
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

                int b2;
                for (auto& tr : a.out(st))
                    if (a.label_of(tr) == letter)
                        b2 = equivalences[a.dst_of(tr)];
                labels[std::make_pair(b1, b2)].push_back(st);
            }

            nbr_eq_classes_last_loop = eq_class;
            eq_class = 0;
            for (auto& label : labels) // foreach state having the same label
            {
                for (auto& st : label.second)
                    equivalences[st] = eq_class;
                ++eq_class;
            }
        }

        for (auto& label : labels) // foreach state having the same label
        {
            std::cout << "(" << label.first.first << ", "
                << label.first.second << ") = ";

            for (auto& st : label.second)
                std::cout << st << " ";
            std::cout << std::endl;
        }
        std::cout << "==============================\n";

        for (auto& eq : equivalences)
            std::cout << eq.first << " => " << eq.second << std::endl;
    }
    while (nbr_eq_classes_last_loop != eq_class);

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
