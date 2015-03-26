#pragma once

#include <unordered_map>

#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/algos/copy.hh> // real_context
#include <vcsn/algos/fwd.hh>
#include <vcsn/misc/pair.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Aut>
    class insplitter
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      using pair_t = std::pair<state_t, bool>;

      // Associate a state with itself and (possibly) the new one from the split
      // false: Original state
      // true: Duplicated via splitting
      std::unordered_map<pair_t, state_t> states_assoc;

    public:
      insplitter(const Aut& aut)
        : res_(make_shared_ptr<automaton_t>(real_context(aut)))
      {}

      automaton_t operator()(const Aut& aut)
      {
        if (!labelset_t_of<Aut>::has_one())
            return aut;

        states_assoc[pair_t(aut->pre(), false)] = res_->pre();
        states_assoc[pair_t(aut->post(), false)] = res_->post();

        for (auto st : aut->states())
        {
          bool epsilon_in = false;
          bool letter_in = false;

          for (auto tr : aut->all_in(st))
          {
            if (is_one(aut, tr))
              epsilon_in = true;
            else
              letter_in = true;
            if (epsilon_in && letter_in)
              break;
          }
          if (epsilon_in)
            states_assoc[pair_t(st, true)] = res_->new_state();
          if (letter_in)
            states_assoc[pair_t(st, false)] = res_->new_state();
        }

        for (auto st : aut->all_states())
          for (bool epsilon : { false, true })
            if (exists(st, epsilon))
              for (auto t : aut->all_out(st))
                res_->add_transition_copy(states_assoc[pair_t(st, epsilon)],
                                          states_assoc[pair_t(aut->dst_of(t),
                                                              is_one(aut, t))],
                                          aut, t);

        return std::move(res_);
      }

    private:
      inline bool exists(state_t st, bool epsilon)
      {
        return states_assoc.find(pair_t(st, epsilon)) != states_assoc.end();
      }

      template <typename A>
      vcsn::enable_if_t<labelset_t_of<A>::has_one(), bool>
      is_one(const A& aut, transition_t tr)
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      template <typename A>
      vcsn::enable_if_t<!labelset_t_of<A>::has_one(), bool>
      is_one(const A&, transition_t)
      {
        raise("lal should not reach this point!");
      }

      automaton_t res_;
    };

    template<typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::has_one(), Aut>
    insplit(Aut& aut)
    {
      insplitter<Aut> insplit{aut};
      return insplit(aut);
    }

    template<typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::has_one(), Aut>
    insplit(Aut& aut)
    {
      return aut;
    }
  } // namespace detail

  template <typename Aut>
  inline
  auto
  insplit(const Aut& aut)
    -> decltype(detail::insplit(aut))
  {
    return detail::insplit(aut);
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
        return make_automaton(::vcsn::insplit(a));
      }
    }
  }

} // namespace vcsn
