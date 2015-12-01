#pragma once

#include <boost/heap/fibonacci_heap.hpp>
#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | Shortest path through Dijkstra algorithm.  |
  `-------------------------------------------*/

  /// Dijkstra implementation (from vcsn/algos/dijkstra.hh).
  ///
  /// Uses fibonacci heap.
  /// No preconditions.
  struct dijkstra_tag {};

  namespace detail
  {
    /// Dijkstra implementation of lightest automaton.
    ///
    /// Functor taking an automaton as parameter and applying
    /// dijkstra algorithm to compute the lightest 'in' transition
    /// of each state.
    template <typename Aut>
    struct dijkstra_impl
    {
      using automaton_t = Aut;
      using self_t = dijkstra_impl;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using distance_t = std::vector<weight_t>;

      dijkstra_impl(const Aut& aut)
        : aut_(aut)
        , res_(aut_->all_states().back() + 1, aut_->null_transition())
        , dist_(aut_->all_states().back() + 1)
      {};

      struct profile
      {
        profile(state_t state, const self_t& d)
          : state_(state)
          , dijkstra_(d)
        {}

        bool operator<(const profile& rhs) const
        {
          auto ws = *dijkstra_.aut_->weightset();
          if (dijkstra_.res_[state_] == dijkstra_.aut_->null_transition())
            return true;
          else if (dijkstra_.res_[rhs.state_] == dijkstra_.aut_->null_transition())
            return false;
          else
            return ws.less(dijkstra_.dist_[rhs.state_], dijkstra_.dist_[state_]);
        }

        friend std::ostream& operator<<(std::ostream& o, const profile& p)
        {
          auto d = p.dijkstra_;
          auto ws = *d.aut_->weightset();
          d.aut_->print_state_name(p.state_, o) << ':';
          if (d.res_[p.state_] != d.aut_->null_transition())
            return ws.print(d.dist_[p.state_], o);
          else
            return o << "null";
        }

        state_t state_;
        const self_t& dijkstra_;
      };

      using heap_t = boost::heap::fibonacci_heap<profile>;

      std::vector<transition_t>
      operator()(state_t source, state_t dest)
      {
        auto size = aut_->all_states().back() + 1;
        auto handles = std::vector<typename heap_t::handle_type>(size);
        auto todo = heap_t();
        auto ws = *aut_->weightset();

        dist_[source] = ws.one();
        handles[source] = todo.emplace(source, *this);

        while (!todo.empty())
          {
            auto p = todo.top();
            todo.pop();
            state_t s = p.state_;
            if (s == dest)
              break;
            else
              for (auto t: aut_->all_out(s))
                {
                  auto dst = aut_->dst_of(t);
                  auto nw = ws.mul(dist_[s], aut_->weight_of(t));
                  if (res_[dst] == aut_->null_transition())
                    {
                      // First visit.
                      dist_[dst] = nw;
                      res_[dst] = t;
                      handles[dst] = todo.emplace(dst, *this);
                    }
                  else if (ws.less(nw, dist_[dst]))
                    {
                      // Lighter path.
                      dist_[dst] = nw;
                      res_[dst] = t;
                      todo.update(handles[dst]);
                    }
                }
          }

        return std::move(res_);
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
      distance_t dist_;
    };
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                dijkstra_tag = {})
  {
    return detail::dijkstra_impl<Aut>(aut)(source, dest);
  }
}
