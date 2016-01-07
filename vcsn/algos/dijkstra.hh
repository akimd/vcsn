#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/tags.hh>
#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | Shortest path through Dijkstra algorithm.  |
  `-------------------------------------------*/

  namespace detail
  {
    /// Dijkstra implementation of lightest automaton.
    ///
    /// Uses Fibonacci heaps.
    /// No preconditions.
    ///
    /// Functor taking an automaton as parameter and applying
    /// dijkstra algorithm to compute the lightest 'in' transition
    /// of each state.
    ///
    /// \tparam ValueSet could be either a labelset or weightset.
    ///         Must have a less and a one member function.
    /// \tparam Mul lambda multiplying the current best candidate with
    ///             the value taken from the transition given in parameter.
    template <Automaton Aut, typename ValueSet, typename Mul>
    struct dijkstra_impl
    {
      using automaton_t = Aut;
      using self_t = dijkstra_impl;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using context_t = context_t_of<automaton_t>;
      using wordset_context_t = word_context_t<context_t>;
      using valueset_t = ValueSet;
      using value_t = typename valueset_t::value_t;
      using distance_t = std::vector<value_t>;

      dijkstra_impl(const Aut& aut, const ValueSet& vs, Mul mul)
        : aut_(aut)
        , res_(aut_->all_states().back() + 1, aut_->null_transition())
        , dist_(aut_->all_states().back() + 1)
        , vs_{vs}
        , mul_{mul}
      {};

      struct profile
      {
        profile(state_t state, const self_t& d)
          : state_(state)
          , self_(d)
        {}

        bool operator<(const profile& rhs) const
        {
          if (self_.res_[state_] == self_.aut_->null_transition())
            return true;
          else if (self_.res_[rhs.state_] == self_.aut_->null_transition())
            return false;
          else
            return self_.vs_.less(self_.dist_[rhs.state_], self_.dist_[state_]);
        }

        state_t state_;
        const self_t& self_;
      };

      using heap_t = boost::heap::fibonacci_heap<profile>;

      std::vector<transition_t>
      operator()(state_t source, state_t dest)
      {
        auto size = aut_->all_states().back() + 1;
        auto handles = std::vector<typename heap_t::handle_type>(size);
        auto todo = heap_t();

        dist_[source] = vs_.one();
        handles[source] = todo.emplace(source, *this);

        while (!todo.empty())
          {
            auto p = todo.top();
            todo.pop();
            state_t s = p.state_;
            if (s == dest)
              break;
            else
              for (auto t: all_out(aut_, s))
                {
                  auto dst = aut_->dst_of(t);
                  auto nv = mul_(dist_[s], t);
                  if (res_[dst] == aut_->null_transition())
                    {
                      // First visit.
                      dist_[dst] = nv;
                      res_[dst] = t;
                      handles[dst] = todo.emplace(dst, *this);
                    }
                  else if (vs_.less(nv, dist_[dst]))
                    {
                      // Lighter path.
                      dist_[dst] = nv;
                      res_[dst] = t;
                      todo.update(handles[dst]);
                    }
                }
          }
        return std::move(res_);
      }

    public:
      const automaton_t& aut_;
      /// For each state, its predecessor.
      std::vector<transition_t> res_;
      distance_t dist_;
      const ValueSet& vs_;
      Mul mul_;
    };

    template <Automaton Aut, typename ValueSet, typename Mul>
    auto
    make_dijkstra_impl(const Aut& aut, const ValueSet& vs, Mul mul)
    {
      return dijkstra_impl<Aut, ValueSet, Mul>(aut, vs, mul);
    }
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                dijkstra_tag)
  {
    auto get_value = [&aut](auto lhs, transition_t_of<Aut> t)
                     {
                       return aut->weightset()->mul(lhs, aut->weight_of(t));
                     };
    auto algo = detail::make_dijkstra_impl(aut, *aut->weightset(), get_value);
    return std::move(algo(source, dest));
  }
}
