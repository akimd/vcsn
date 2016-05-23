#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/misc/set.hh>
#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{
  /*-----------------------------------------.
  | Shortest path through A-Star algorithm.  |
  `-----------------------------------------*/

  /// A-Star implementation (from vcsn/algos/a-star.hh).
  ///
  /// Uses neutral heuristic.
  /// No preconditions.
  struct a_star_tag {};

  namespace detail
  {
    /// A Star implementation of lightest automaton.
    ///
    /// Functor taking an automaton and an a-star heuristic as parameter,
    /// applying a-star on the given automaton with the given heuristic.
    /// Return an array corresponding to the lightest 'in' transition of
    /// every state indexed by their state number.
    template <Automaton Aut>
    struct a_star_impl
    {
      using automaton_t = Aut;
      using self_t = a_star_impl;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using distance_t = std::vector<weight_t>;

      a_star_impl(const Aut& aut)
        : aut_(aut)
        , res_(states_size(aut_), aut_->null_transition())
        , heuristic_dist_(states_size(aut_))
      {};

      struct profile
      {
        profile(state_t state, const self_t& a_star)
          : state_(state)
          , a_star_(a_star)
        {}

        bool operator<(const profile& rhs) const
        {
          auto ws = *a_star_.aut_->weightset();
          if (a_star_.res_[state_] == a_star_.aut_->null_transition())
            return true;
          else if (a_star_.res_[rhs.state_] == a_star_.aut_->null_transition())
            return false;
          else
            return ws.less(a_star_.heuristic_dist_[rhs.state_],
                           a_star_.heuristic_dist_[state_]);
        }

        friend std::ostream& operator<<(std::ostream& o, const profile& p)
        {
          auto a = p.a_star_;
          auto ws = *a.aut_->weightset();
          a.aut_->print_state_name(p.state_, o) << ':';
          if (a.res_[p.state_] != a.aut_->null_transition())
            return ws.print(a.heuristic_dist_[p.state_], o);
          else
            return o << "null";
        }

        state_t state_;
        const self_t& a_star_;
      };

      using heap_t = boost::heap::fibonacci_heap<profile>;

      template <typename Heuristic>
      std::vector<transition_t>
      operator()(state_t source, state_t dest, Heuristic heuristic)
      {
        auto ws = *aut_->weightset();
        auto size = states_size(aut_);

        auto done = std::set<state_t>();

        auto todo = heap_t();
        std::vector<typename heap_t::handle_type> handles(size);

        auto dist = distance_t(size);

        dist[source] = ws.one();
        heuristic_dist_[source] = ws.add(ws.one(), heuristic(source, dest));
        handles[source] = todo.emplace(source, *this);

        while (!todo.empty())
          {
            auto p = todo.top();
            state_t s = p.state_;
            if (s == dest)
              return std::move(res_);
            todo.pop();
            done.insert(s);
            for (auto t: all_out(aut_, s))
              {
                auto dst = aut_->dst_of(t);
                if (!has(done, dst))
                  {
                    auto nw = ws.mul(dist[s], aut_->weight_of(t));
                    if (res_[dst] == aut_->null_transition()
                        || ws.less(nw, dist[dst]))
                      {
                        res_[dst] = t;
                        dist[dst] = nw;
                        heuristic_dist_[dst] = ws.add(nw, heuristic(dst, dest));
                      }
                    handles[dst] = todo.emplace(dst, *this);
                  }
              }
          }

        return std::vector<transition_t>(size, aut_->null_transition());
      }

    private:
      void show_heap_(const heap_t& todo)
      {
        const char* sep = "";
        for (auto i = todo.ordered_begin(), end = todo.ordered_end();
             i != end; ++i)
          {
            std::cout << sep << *i;
            sep = ", ";
          }
        std::cout << std::endl;
      }

    public:
      const automaton_t& aut_;
      /// For each state, its predecessor.
      std::vector<transition_t> res_;
      distance_t heuristic_dist_;
    };
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                a_star_tag)
  {
    using state_t = state_t_of<Aut>;
    return detail::a_star_impl<Aut>(aut)(source, dest,
                                         [aut](state_t, state_t)
                                         {
                                           return aut->weightset()->zero();
                                         });
  }
}
