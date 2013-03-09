#ifndef VCSN_ALGOS_IS_EPS_ACYCLIC
# define VCSN_ALGOS_IS_EPS_ACYCLIC

# include <unordered_map>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/fwd.hh>

namespace vcsn
{
  namespace
  {
    /**
      @class epsilon_acylic
      @brief This class provides an algorithm to detect epsilon-circuits.

      In this algorithm, only epsilon-transitions are considered.

    */
    template <typename Aut>
    struct epsilon_acyclic
    {
      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using label_t = typename automaton_t::label_t;
      std::unordered_map<state_t, char> tag;
      /*
         tag gives the status of the state s;
         if s is not in the map, the state has not been reached yet;
         if tag[s]='u', the status is unknown, the graph reachable from s is
           under exploration
         if tag[s]='o', the status is "ok": there is no eps-circuit accessible
           from s
         if tag[s]='c', there is an eps-circuit accessible from s
         */

      const automaton_t& input;
      label_t empty_word;
      //Return true if an epsilon-circuit is accessible from s by
      //epsilon-transitions.
      bool has_epsilon_circuit(state_t s)
      {
        auto it = tag.find(s);
        if (it == tag.end())
        {
          tag[s] = 'u';
          for (auto t : input.out(s, empty_word))
            if (has_epsilon_circuit(input.dst_of(t)))
            {
              tag[s] = 'c';
              return true;
            }
          tag[s] = 'o';
          return false;
        }

        switch (it->second)
        {//switch with respect to tag[s]
          case 'u':
            /* s is reached while we are exploring successors of s : there is a
            ** circuit */
            tag[s] = 'c';
            return true;
            /* otherwise the graph reachable from s has already been explored */
          case 'o':
            return false;
          default: //'c'
            return true;
        }
      }

      epsilon_acyclic(const automaton_t& input)
        : input(input), empty_word(input.labelset()->empty_word())
      {
      }

      bool is_eps_acyclic()
      {
        for (auto s : input.states())
          if (has_epsilon_circuit(s))
            return false;
        return true;
      }
    };
  }


  template <class Aut>
  bool is_eps_acyclic(const Aut& input)
  {
    epsilon_acyclic<Aut> t{input};
    return t.is_eps_acyclic();
  }

  /*---------------------.
  | dyn::is_eps_acyclic. |
  `---------------------*/

  namespace dyn
  {
    namespace details
    {

      template <typename Aut>
      bool is_eps_acyclic(const dyn::automaton& aut)
      {
        return is_eps_acyclic(dynamic_cast<const Aut&>(*aut));
      }

      using is_eps_acyclic_t = bool(const dyn::automaton&);
      bool is_eps_acyclic_register(const std::string& ctx,
                                    const is_eps_acyclic_t& fn);

    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_IS_EPS_ACYCLIC
