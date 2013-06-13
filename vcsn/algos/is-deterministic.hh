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
    using label_t = typename automaton_t::labelset_t::label_t;

    static_assert(automaton_t::context_t::is_lal,
                  "requires labels_are_letters");

    // States to visit.
    std::queue<state_t> q;
    // States already visited (or already in q).
    std::unordered_set<label_t> seen;
    std::unordered_set<state_t> marked;

    q.push(aut.pre());
    marked.insert(aut.pre());

    while (!q.empty())
      {
        state_t st = std::move(q.front());
        q.pop();

        seen.clear();
        for (auto tr : aut.all_out(st))
          {
            state_t succ = aut.dst_of(tr);
            const label_t& wd = aut.label_of(tr);
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
    namespace detail
    {
      template <typename Aut>
      bool
      is_deterministic(const automaton& aut)
      {
        return is_deterministic(dynamic_cast<const Aut&>(*aut));
      }

      REGISTER_DECLARE(is_deterministic,
                       (const automaton& aut) -> bool);
    }
  }
} // namespace vscn

#endif // !VCSN_ALGOS_IS_DETERMINISTIC_HH
