#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <iostream>
# include <map>
# include <mutex>
# include <queue>
# include <stack>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

# include <tbb/parallel_for_each.h>

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

    if (!is_deterministic(a))
      throw std::domain_error("minimize: requires a deterministic automaton");

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

    std::map<state_t, int> equivalences;


    // Init. Eq[0] = finals, Eq[1] = others.

    for (auto st : a.states())
      equivalences[st] = (a.is_final(st) ? 1 : 2);

    std::map<std::pair<int, int>, std::vector<state_t>> labels;

    std::mutex labels_defender;

    int eq_class = 2;
    int nbr_eq_classes_last_loop = 2;
    do
    {
      for (auto letter : letters)
      {
        labels.clear();
        tbb::parallel_for_each(a.states().begin(), a.states().end(),
                [&](state_t st) -> void
        {
          // Here we test for each letter start and end equivalence
          // class, then label the state
          int b1 = equivalences[st];

          int b2 = -1;
          for (auto& tr : a.out(st))
            if (a.label_of(tr) == letter)
              b2 = equivalences[a.dst_of(tr)];

          if (b2 != -1)
          {
            labels_defender.lock();
            labels[std::make_pair(b1, b2)].push_back(st);
            labels_defender.unlock();
          }
        });

        nbr_eq_classes_last_loop = eq_class;
        eq_class = 2;
        for (auto& label : labels) // foreach state having the same label
        {
          for (auto& st : label.second)
            equivalences[st] = eq_class;
          ++eq_class;
        }
      }
    }
    while (nbr_eq_classes_last_loop != eq_class);

    std::map<int, state_t> eq_to_res;
    equivalences[a.pre()] = 0;
    equivalences[a.post()] = 1;
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
