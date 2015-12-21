#pragma once

#include <array>
#include <vector>

#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/algorithm.hh> // detail::back
#include <vcsn/misc/tuple.hh> // make_index_sequence

namespace vcsn
{

  namespace detail
  {
    template <Automaton Aut>
    class bounded_lag_checker
    {
      static_assert(context_t_of<Aut>::is_lat,
                    "has_bounded_lag: automaton labelset must be a tupleset");
      // The algorithm makes no sense (and doesn't compile) if the transducer
      // has only one tape.
      static_assert(labelset_t_of<Aut>::size() >= 2,
                    "has_bounded_lag: transducer labelset must have at least 2 tapes");

      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using label_t = typename labelset_t::value_t;
      using transition_t = transition_t_of<automaton_t>;

      static constexpr size_t number_of_tapes = labelset_t_of<Aut>::size();

      enum visit_state {
        NOT_VISITED,
        VISITING,
        VISITED
      };

      // Keep track of what states we have visited or are visiting
      using visited_t = std::vector<visit_state>;

      // Keep track of how we arrived in a state
      using parent_state_t = std::vector<transition_t>;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      using delay_index_t = detail::make_index_sequence<number_of_tapes - 1>;

      /// The delay associated with each state
      using delay_t = std::array<int, number_of_tapes - 1>;

    public:

      // The vectors are indexed by the state number
      bounded_lag_checker(const automaton_t& aut)
        : aut_(aut), v_(detail::back(aut_->all_states()) + 1, NOT_VISITED)
        , p_(detail::back(aut_->all_states()) + 1, -1)
      {}

      // Get the size (number of letters) of a label on a specific tape.
      template <std::size_t I>
      int get_size_tape(label_t l)
      {
        using tape_labelset_t = typename labelset_t::template valueset_t<I>;
        return tape_labelset_t::size(std::get<I>(l));
      }

      // Add the delay from the transition's label to the given delay
      template <std::size_t... I>
      void add_delay(delay_t& d, transition_t tr, seq<I...>)
      {
        label_t l = aut_->label_of(tr);
        int i0 = get_size_tape<0>(l);
        d = {(d[I] + i0 - get_size_tape<I + 1>(l))...};
      }

      void add_delay(delay_t& d, transition_t tr)
      {
        add_delay(d, tr, delay_index_t{});
      }

      // Depth-first search
      bool has_bounded_lag(state_t src)
      {
        v_[src] = VISITING;
        for (auto tr : aut_->all_out(src))
          {
            state_t dst = aut_->dst_of(tr);
            auto visited = v_[dst];
            if (visited == NOT_VISITED)
              {
                p_[dst] = tr;
                if (!has_bounded_lag(dst))
                  return false;
              }
            else if (visited == VISITING)
              {
                // Cycle, compute the cycle's delay
                delay_t d = {0};
                add_delay(d, tr);
                if (src != dst)
                  {
                    transition_t t = p_[src];
                    // Go back through the transitions we followed until we
                    // loop
                    while (aut_->src_of(t) != dst)
                      {
                        add_delay(d, t);
                        t = p_[aut_->src_of(t)];
                      }
                    add_delay(d, t);
                  }
                if (d != delay_t{0})
                  return false;
              }
          }
        v_[src] = VISITED;
        return true;
      }

      bool has_bounded_lag()
      {
        return has_bounded_lag(aut_->pre());
      }

    private:
      automaton_t aut_;
      visited_t v_;
      parent_state_t p_;
    };
  }


  /*------------------.
  | has_bounded_lag.  |
  `------------------*/

  /// Whether a transducer has a bounded lag.
  ///
  /// \param[in] aut        the transducer
  /// \returns              whether the lag is bounded
  template <Automaton Aut>
  bool has_bounded_lag(const Aut& aut)
  {
    detail::bounded_lag_checker<Aut> blc(aut);
    return blc.has_bounded_lag();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool has_bounded_lag(const automaton& aut)
      {
        return has_bounded_lag(aut->as<Aut>());
      }
    }
  }

}
