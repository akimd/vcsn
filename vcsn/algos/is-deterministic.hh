#ifndef VCSN_ALGOS_IS_DETERMINISTIC_HH
# define VCSN_ALGOS_IS_DETERMINISTIC_HH

# include <queue>
# include <unordered_set>

# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  template <class Aut>
  inline bool
  is_deterministic(const Aut& aut)
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using word_t = typename automaton_t::labelset_t::word_t;

    std::queue<state_t> q;
    std::unordered_set<state_t> marked;
    std::unordered_set<word_t> seen;
    word_t wd;

    for (auto init : aut.initial_transitions())
      {
        q.push(init);
        marked.insert (init);
      }

    while (!q.empty())
      {
        state_t st = q.front();
        q.pop();

        seen.clear();
        for (auto tr : aut.all_out(st))
          {
            state_t succ = aut.dst_of(tr);
            wd = aut.word_label_of(tr);
            if (!seen.insert(wd).second)
              return false;

            if (marked.find(succ) == marked.end())
              {
                q.push(succ);
                marked.insert(succ);
              }
          }
      }

    return true;
  }

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      bool
      is_deterministic(const dyn::automaton& aut)
      {
        return is_deterministic(dynamic_cast<const Aut&>(*aut));
      }

      REGISTER_DECLARE(is_deterministic,
                       (const dyn::automaton& aut) -> bool);
    }
  }
} // namespace vscn

#endif // !VCSN_ALGOS_IS_DETERMINISTIC_HH
