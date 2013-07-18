#ifndef VCSN_ALGOS_PROPER_HH
# define VCSN_ALGOS_PROPER_HH

# include <stdexcept>
# include <type_traits>
# include <unordered_map>
# include <utility>
# include <vector>

# include <vcsn/algos/fwd.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/is-proper.hh>
# include <vcsn/algos/is-valid.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/direction.hh>

namespace vcsn
{

  namespace detail
  {
    /**
       @class properer
       @brief This class contains the core of the proper algorithm.

       This class is specialized for labels_are_letter automata since all these
       methods become trivial.

    */

    template <typename Aut, typename Kind = typename Aut::kind_t>
    class properer
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = typename automaton_t::state_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;
      using label_t = typename automaton_t::label_t;
      using transition_t = typename automaton_t::transition_t;
      using kind_t = Kind;

      /**
         @brief The core of the (backward) epsilon-removal.

         For each state s
         if s has an epsilon-loop with weight w
         if w is not starrable, return false
         else compute ws = star(w)
         endif
         remove the loop
         else
         ws = 1
         endif
         for each incoming epsilon transition e:p-->s with weight h
         for each outgoing transition s--a|k-->q
         add (and not set) transition p-- a | h.ws.k --> q
         endfor
         if s is final with weight h
         add final weight h.ws to p
         endif
         remove e
         endfor
         endfor
         return true

         If the method returns false, \a aut is corrupted.

         @param aut The automaton in which epsilon-transitions will be removed
         @return true if the proper succeeds, or false otherwise.
      */

    public:
      static bool in_situ_remover(automaton_t& aut)
      {
        // States to erase if (i) they had incoming spontaneous
        // transitions, and (ii) after completion of the process, they
        // have no incoming transition.
        std::vector<state_t> dead_states;

        label_t empty_word = aut.labelset()->one();
        const auto& weightset = *aut.weightset();
        using state_weight_t = std::pair<state_t, weight_t>;
        std::vector<state_weight_t> closure;

        /* For each state (s), for each incoming epsilon-transitions
           (t), if (t) is a loop, the star of its weight is computed,
           otherwise, (t) is stored into the closure list.  Then (t)
           is removed.  */
        for (auto s: aut.states())
          {
            weight_t star = weightset.one();// if there is no eps-loop
            closure.clear();
            const auto& tr = aut.in(s, empty_word);
            if (!tr.empty())
              {
                // Iterate on a copy, as we remove these transitions
                // in the loop.
                std::vector<transition_t> transitions{tr.begin(), tr.end()};
                for (auto t : transitions)
                  {
                    weight_t t_weight = aut.weight_of(t);
                    state_t src = aut.src_of(t);
                    if (src == s)  //loop
                      try
                        {
                          star = weightset.star(t_weight);
                        }
                      catch (const std::domain_error&)
                        {
                          return false;
                        }
                    else
                      closure.emplace_back(src, t_weight);
                    // Delete incoming epsilon transitions.
                    aut.del_transition(t);
                  }
                // Maybe schedule for removal.
                if (aut.all_in(s).empty())
                  dead_states.emplace_back(s);
              }

            /*
              For each transition (t : s -- label|weight --> dst),
              for each former
              epsilon transition closure->first -- e|closure->second --> s
              a transition
              (closure->first -- label | closure->second*weight --> dst)
              is added to the automaton (add, not set !!)

              If (s) is final with weight (weight),
              for each former
              epsilon transition closure->first -- e|closure->second --> s
              pair-second * weight is added to the final weight
              of closure->first
            */
            for (auto t: aut.all_out(s))
              {
                // "Blowing": For each transition (or terminal arrow)
                // outgoing from (s), the weight is multiplied by
                // (star).
                weight_t weight = weightset.mul(star, aut.weight_of(t));
                aut.set_weight(t, weight);

                label_t label = aut.label_of(t);
                state_t dst = aut.dst_of(t);
                for (auto pair: closure)
                  aut.add_transition(pair.first, dst, label,
                                     weightset.mul(pair.second, weight));
              }
          }
        for (auto s: dead_states)
          if (aut.all_in(s).empty())
            aut.del_state(s);
        return true;
      }

      /**@brief Remove the epsilon-transitions of the input
         The behaviour of this method depends on the star_status of the weight_set:
         -- starrable : always valid, does not throw any exception
         -- tops : the proper algo is directly launched on the input;
         if it returns false, an exception is launched
         -- non_starrable / absval:
         is_valid is called before launching the algorithm.
         @param aut The automaton in which epsilon-transitions will be removed
         @throw domain_error if the input is not valid
      */
      static void proper_here(automaton_t& aut)
      {
        if (!is_proper(aut))
          proper_here_<weightset_t::star_status()>(aut);
      }

      static automaton_t proper(const automaton_t& aut)
      {
        automaton_t res = copy(aut);
        proper_here(res);
        return res;
      }

    private:
      /// TOPS: valid iff proper succeeds.
      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::TOPS>::type
      proper_here_(automaton_t& aut)
      {
        if (!in_situ_remover(aut))
          throw std::domain_error("invalid automaton");
      }

      /// ABSVAL: valid iff proper succeeds on the "absolute value" of
      /// the automaton.
      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::ABSVAL>::type
      proper_here_(automaton_t& aut)
      {
        if (!is_valid(aut))
          throw std::domain_error("invalid automaton");
        in_situ_remover(aut);
      }

      /// STARRABLE: always valid.
      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::STARRABLE>::type
      proper_here_(automaton_t& aut)
      {
        in_situ_remover(aut);
      }

      /// NON_STARRABLE: valid iff there is no epsilon-circuit with
      /// weight zero.  Warning: the property tested here is the
      /// acyclicity, which is equivalent only in zero divisor free
      /// semirings.
      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::NON_STARRABLE>::type
      proper_here_(automaton_t& aut)
      {
        if (!is_valid(aut))
          throw std::domain_error("invalid automaton");
        in_situ_remover(aut);
      }
    };


    /*---------------------.
    | labels_are_letters.  |
    `---------------------*/

    template <typename Aut>
    class properer<Aut, labels_are_letters>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
    public:
      static
#ifndef __clang__
      constexpr
#endif
      void proper_here(automaton_t&)
      {}

      static Aut proper(const automaton_t& aut)
      {
        return copy(aut);
      }
    };

  }


  /*---------.
  | proper.  |
  `---------*/

  template <typename Aut>
  inline
  bool in_situ_remover(Aut& aut)
  {
    return detail::properer<Aut>::in_situ_remover(aut);
  }

  template <class Aut>
  inline
  void proper_here(Aut& aut, direction_t dir = direction_t::BACKWARD)
  {
    switch (dir)
      {
      case direction_t::BACKWARD:
        detail::properer<Aut>::proper_here(aut);
        return;
      case direction_t::FORWARD:
        auto tr_aut = transpose(aut);
        using tr_aut_t = decltype(tr_aut);
        detail::properer<tr_aut_t>::proper_here(tr_aut);
        return;
      }
  }

  template <class Aut>
  Aut proper(const Aut& aut, direction_t dir = direction_t::BACKWARD)
  {
    switch (dir)
      {
      case direction_t::BACKWARD:
        return detail::properer<Aut>::proper(aut);
      case direction_t::FORWARD:
        // FIXME: inconsistent implementation bw fwd and bwd.
        Aut res = copy(aut);
        proper_here(res, dir);
        return res;
      }
    abort();
  }

  namespace dyn
  {

    /*--------------.
    | dyn::proper.  |
    `--------------*/

    namespace detail
    {
      template <typename Aut>
      automaton proper(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), proper(a));
      }

     REGISTER_DECLARE(proper, (const automaton& aut) -> automaton);
    }

  }

} // namespace vcsn

#endif // !VCSN_ALGOS_PROPER_HH
