#ifndef VCSN_ALGOS_ACCESSIBLE_HH
# define VCSN_ALGOS_ACCESSIBLE_HH

# include <map>
# include <set>
# include <stack>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <class Aut>
  Aut accessible(const Aut& a)
  {
    // FIXME Static assert for label's type
    using automaton_t = Aut;
    using state_t = typename Aut::state_t;
    // map associating source's states to result's states
    using map = std::map<state_t, state_t>;
    // We are exploring the automaton. We can also use a queue.
    using stack = std::stack<state_t>;

    map seen;
    stack todo;

    automaton_t res{a.context()};

    todo.push(a.pre());
    // pre and post are automatically created, so we just register them
    seen[a.pre()] = res.pre();
    seen[a.post()] = res.post();

    while (!todo.empty())
    {
      const state_t src = todo.top();
      todo.pop();

      state_t creating_node = seen[src];

      for (auto tr : a.all_out(src))
      {
        state_t dst = a.dst_of(tr);
        // if the dst state does not exists in the res, let's create it
        auto inserted = seen.insert(typename map::value_type(dst, a.null_state()));
        if (inserted.second)
        {
          todo.push(dst);
          inserted.first->second = res.new_state();
        }

        res.add_transition(src, seen[dst], a.label_of(tr),
            a.weight_of(tr));
      }
    }

    return res;
  }

  /*-----------------.
  | dyn::accessible. |
  `-----------------*/

  namespace dyn
  {
    namespace details
    {
      template <class Aut>
      dyn::automaton
      accessible(const dyn::automaton& aut)
      {
        return std::make_shared<Aut>(
            accessible(dynamic_cast<const Aut&>(*aut)));
      }

      using accessible_t =
        auto (const dyn::automaton&) -> dyn::automaton;
      bool accessible_register(const std::string& ctx, const accessible_t& fn);
    }
  }
}

#endif // !VCSN_ALGOS_ACCESSIBLE_HH
