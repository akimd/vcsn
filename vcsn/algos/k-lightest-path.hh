#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/dijkstra.hh>
#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{
  struct yen_tag {};

  namespace detail
  {
    template <Automaton Aut>
    struct yen_impl
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using path_t = std::vector<transition_t>;
      using paths_t = std::vector<path_t>;

      yen_impl(const automaton_t& aut)
        : aut_(aut)
      {}

      struct profile
      {
        profile(const path_t& path, const weight_t& w, const automaton_t& aut)
          : path_(path)
          , w_(w)
          , aut_(aut)
        {}

        bool operator<(const profile& rhs) const
        {
          return aut_->weightset()->less(rhs.w_, w_);
        }

        path_t path_;
        weight_t w_;
        const automaton_t& aut_;
      };

      using heap_t = boost::heap::fibonacci_heap<profile>;

      path_t
      path(const Aut& aut,
           const path_t& path,
           state_t_of<Aut> src = Aut::element_type::pre(),
           state_t_of<Aut> dst = Aut::element_type::post())
      {
        path_t res;
        for (auto t = path[dst];
             t != aut->null_transition();
             t = path[aut->src_of(t)])
          {
            res.insert(res.begin(), t);
            if (aut->src_of(t) == src)
              break;
          }
        return res;
      }

      paths_t
      operator()(state_t src, state_t dst, unsigned k)
      {
        auto res = paths_t{path(aut_, lightest_path(aut_, src, dst, dijkstra_tag{}), src, dst)};

        auto heap = heap_t();

        for (unsigned i = 1u; i < k; i++)
          {
            const auto& prev = res[i - 1];
            for (unsigned j = 0u; j < prev.size(); j++)
              {
                auto copy_aut = make_fresh_automaton(aut_);
                auto copier = make_copier(aut_, copy_aut);
                copier();
                auto spur_node = copier.state_map()[aut_->src_of(prev[j])];
                auto root_path = path_t(prev.begin(), prev.begin() + j);

                for (const auto& selected_path: res)
                  if (j < selected_path.size())
                    {
                      auto diff = std::mismatch(root_path.begin(), root_path.end(),
                                                selected_path.begin(), selected_path.begin() + j);
                      if (diff.first == root_path.end()
                          && copy_aut->has_transition(selected_path[j]))
                        copy_aut->del_transition(selected_path[j]);
                    }

                for (auto t: root_path)
                  if (t != aut_->null_transition()
                      && copy_aut->src_of(t) != spur_node
                      && copy_aut->src_of(t) != aut_->pre()
                      && copy_aut->has_state(copy_aut->src_of(t)))
                    copy_aut->del_state(copy_aut->src_of(t));

                auto shortest_path = lightest_path(copy_aut, spur_node, dst, dijkstra_tag{});
                auto spur_path = path(copy_aut, shortest_path, spur_node, dst);
                root_path.insert(root_path.end(), spur_path.begin(), spur_path.end());
                if (!root_path.empty()
                  && copy_aut->src_of(root_path.front()) == src
                  && copy_aut->dst_of(root_path.back()) == dst)
                  {
                    weight_t weight = aut_->weightset()->one();
                    for (auto t : root_path)
                      weight = aut_->weightset()->mul(weight, aut_->weight_of(t));
                    heap.emplace(std::move(root_path), weight, aut_);
                  }
              }
            if (heap.empty())
              break;
            res.push_back(heap.top().path_);
            heap.pop();
          }

        return res;
      }

      const automaton_t& aut_;
    };
  }

  template <Automaton Aut>
  std::vector<std::vector<transition_t_of<Aut>>>
  k_lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest, unsigned k)
  {
    return detail::yen_impl<Aut>(aut)(source, dest, k);
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  format_lightest(const Aut& aut, const std::vector<transition_t_of<Aut>>& path)
  {
    auto res = std::vector<transition_t_of<Aut>>(aut->all_states().back() + 1,
                                                 aut->null_transition());
    for (auto t : path)
      if (t != aut->null_transition())
        res[aut->dst_of(t)] = t;
    return res;
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                yen_tag)
  {
    auto paths = k_lightest_path(aut, source, dest, 1);
    auto res = paths.empty() ? std::vector<transition_t_of<Aut>>() : paths.front();
    return format_lightest(aut, res);
  }
}
