#ifndef VCSN_ALGOS_INSPLITTING_HH
# define VCSN_ALGOS_INSPLITTING_HH

# include <unordered_map>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/copy.hh>
# include <vcsn/misc/pair.hh>
# include <vcsn/misc/attributes.hh>

namespace vcsn
{
  namespace detail
  {
    template <class Aut>
    class insplitter
    {
      static_assert(Aut::context_t::is_lal || Aut::context_t::is_lan,
                    "requires labels_are_letters or labels_are_nullable");

      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using label_t = typename automaton_t::label_t;
      using transition_t = typename automaton_t::transition_t;

      using pair_t = typename std::pair<state_t, bool>;

      // Associate a state with itself and (possibly) the new one from the split
      // false: Original state
      // true: Duplicated via splitting
      std::unordered_map<pair_t, state_t> states_assoc;

    public:

      insplitter (const Aut& aut)
        : res(aut.context())
      {}

      Aut split(const Aut& aut)
      {
        if (!Aut::context_t::labelset_t::has_one())
          return copy(aut);

        states_assoc[pair_t(aut.pre(), false)] = res.pre();
        states_assoc[pair_t(aut.post(), false)] = res.post();

        for (auto st : aut.states())
        {
          bool epsilon_in = false;
          bool letter_in = false;

          for (auto tr : aut.all_in(st))
          {
            if (is_one(aut, tr))
              epsilon_in = true;
            else
              letter_in = true;
            if (epsilon_in && letter_in)
              break;
          }
          if (epsilon_in)
            states_assoc[pair_t(st, true)] = res.new_state();
          if (letter_in)
            states_assoc[pair_t(st, false)] = res.new_state();
        }

        for (auto st : aut.all_states())
          for (bool epsilon : { false, true })
            if (exists(st, epsilon))
              for (auto tr : aut.all_out(st))
                res.add_transition(states_assoc[pair_t(st, epsilon)],
                                   states_assoc[pair_t(aut.dst_of(tr),
                                                       is_one(aut, tr))],
                                   aut.label_of(tr), aut.weight_of(tr));

        return std::move(res);
      }
    private:

      inline bool exists(state_t st, bool epsilon)
      {
        return states_assoc.find(pair_t(st, epsilon)) != states_assoc.end();
      }

      template <typename A>
      typename std::enable_if<A::context_t::labelset_t::has_one(),
                              bool>::type
      is_one(const A& aut, transition_t tr)
      {
        return aut.labelset()->is_one(aut.label_of(tr));
      }

      template <typename A>
      typename std::enable_if<!A::context_t::labelset_t::has_one(),
                              bool>::type
      is_one(const A& aut ATTRIBUTE_UNUSED, transition_t tr ATTRIBUTE_UNUSED)
      {
        raise("lal should not reach this point!");
      }


      automaton_t res;
    };

  } // namespace detail

  template <typename Aut>
  inline
  Aut
  insplit(const Aut& aut)
  {
    detail::insplitter<Aut> insplit{aut};
    return insplit.split(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      insplit(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(insplit(a));
      }

      REGISTER_DECLARE(insplit,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_INSPLITTING_HH
