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
      using transitions_t = std::vector<transition_t>;
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
      properer(automaton_t& aut)
        : aut_(aut)
        , ws_(*aut.weightset())
        , empty_word_(aut.labelset()->one())
      {}

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

      static bool in_situ_remover(automaton_t& aut)
      {
        try
          {
            properer p(aut);
            p.in_situ_remover_();
            return true;
          }
        catch (const std::domain_error&)
          {
            return false;
          }
      }

      /* For each state (s), for each incoming epsilon-transitions
         (t), if (t) is a loop, the star of its weight is computed,
         otherwise, (t) is stored into the closure list.  Then (t) is
         removed.  */
      // Iterate on a copy, as we remove these transitions in the
      // loop.
      void in_situ_remover_(state_t s, transitions_t transitions)
      {
        weight_t star = ws_.one();// if there is no eps-loop
        using state_weight_t = std::pair<state_t, weight_t>;
        std::vector<state_weight_t> closure;
        for (auto t : transitions)
          {
            weight_t weight = aut_.weight_of(t);
            state_t src = aut_.src_of(t);
            if (src == s)  //loop
              star = ws_.star(weight);
            else
              closure.emplace_back(src, weight);
            // Delete incoming epsilon transitions.
            aut_.del_transition(t);
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
        for (auto t: aut_.all_out(s))
          {
            // "Blowing": For each transition (or terminal arrow)
            // outgoing from (s), the weight is multiplied by
            // (star).
            weight_t weight = ws_.mul(star, aut_.weight_of(t));
            aut_.set_weight(t, weight);

            label_t label = aut_.label_of(t);
            state_t dst = aut_.dst_of(t);
            for (auto pair: closure)
              aut_.add_transition(pair.first, dst, label,
                                  ws_.mul(pair.second, weight));
          }
        if (aut_.all_in(s).empty())
          aut_.del_state(s);
      }

      void in_situ_remover_()
      {
        /* For each state (s), for each incoming epsilon-transitions
           (t), if (t) is a loop, the star of its weight is computed,
           otherwise, (t) is stored into the closure list.  Then (t)
           is removed.  */
        for (auto s: aut_.states())
          {
            const auto& tr = aut_.in(s, empty_word_);
            if (!tr.empty())
              in_situ_remover_(s, {tr.begin(), tr.end()});
          }
      }

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

    private:
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_;
      /// Shorthand to the empty word.
      label_t empty_word_;
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
        const auto& a = aut->as<Aut>();
        return make_automaton(a.context(), proper(a));
      }

     REGISTER_DECLARE(proper, (const automaton& aut) -> automaton);
    }

  }

} // namespace vcsn

#endif // !VCSN_ALGOS_PROPER_HH
