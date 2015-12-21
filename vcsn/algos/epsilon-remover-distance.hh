#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/distance.hh>

namespace vcsn
{
  namespace detail
  {
    /// This class contains the core of the proper algorithm.
    ///
    /// This class is specialized for labels_are_letter automata since
    /// all these methods become trivial.
    template <Automaton Aut,
              bool has_one = labelset_t_of<Aut>::has_one()>
    class epsilon_remover_distance
    {
      using automaton_t = std::remove_cv_t<Aut>;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using labelset_t = labelset_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      /// Context for spontaneous automaton (only spontaneous
      /// transitions of the input automaton).
      using dirty_ctx_t = context<vcsn::oneset, weightset_t>;
      /// Spontaneous automaton.
      using aut_dirty_t = mutable_automaton<dirty_ctx_t>;
      using state_dirty_t = state_t_of<aut_dirty_t>;

      /// Context for proper automaton (only proper transitions).
      using proper_ctx_t = detail::proper_context<context_t_of<Aut>>;
      /// Proper automaton.
      using aut_proper_t = fresh_automaton_t_of<Aut, proper_ctx_t>;
      using state_proper_t = state_t_of<aut_proper_t>;
      using label_proper_t = label_t_of<aut_proper_t>;

    public:
      epsilon_remover_distance(const automaton_t& aut, bool prune = true)
        : ws_(*aut->weightset())
        , prune_(prune)
        , d2p_(aut->all_states().back() + 1,
               aut_proper_t::element_type::null_state())
        , p2d_(aut->all_states().back() + 1,
               aut_dirty_t::element_type::null_state())
      {
        auto dirty_ctx = dirty_ctx_t{{}, ws_};
        auto proper_ctx = make_proper_context(aut->context());
        aut_dirty_ = make_shared_ptr<aut_dirty_t>(dirty_ctx);
        aut_proper_ = make_shared_ptr<aut_proper_t>(proper_ctx);

        auto pcopier = make_copier(aut, aut_proper_);
        auto dcopier = make_copier(aut, aut_dirty_);

        pcopier([](state_t) { return true; },
                [&aut](transition_t t) {
                  return !aut->labelset()->is_one(aut->label_of(t));
                }
        );
        dcopier([&aut](state_t s) {
                  return (!in(aut, s, aut->labelset()->one()).empty()
                         || !out(aut, s, aut->labelset()->one()).empty());
                },
                [&aut](transition_t t) {
                  return aut->labelset()->is_one(aut->label_of(t));
                }
        );

        const auto& dorigins = dcopier.state_map();
        const auto& porigins = pcopier.state_map();
        for (const auto& dp : dorigins)
          {
            auto pp = porigins.find(dp.first);
            assert(pp != porigins.end());
            d2p_[dp.second] = pp->second;
            p2d_[pp->second] = dp.second;
          }

        de_ = all_distances(aut_dirty_);
      }

    public:

      aut_proper_t operator()()
      {
        for (auto dirty_p : aut_dirty_->states())
          for (auto dirty_q : aut_dirty_->states())
            {
              weight_t dist_pq = de_[dirty_p][dirty_q];
              if (!ws_.is_zero(dist_pq))
                {
                  auto proper_q = d2p_[dirty_q];
                  for (auto t : aut_proper_->all_out(proper_q))
                    {
                      auto proper_p = d2p_[dirty_p];
                      auto proper_r = aut_proper_->dst_of(t);
                      label_proper_t l = aut_proper_->label_of(t);
                      weight_t w = aut_proper_->weight_of(t);
                      aut_proper_->set_transition(proper_p, proper_r, l,
                                                  ws_.mul(dist_pq, w));
                    }
                }
            }
        if (prune_)
          for (auto s : aut_proper_->states())
            if (aut_proper_->all_in(s).empty())
              aut_proper_->del_state(s);
        return aut_proper_;
      }

    private:
      /// The automata we work on.
      aut_proper_t aut_proper_;
      aut_dirty_t aut_dirty_;

      /// Shorthand to the weightset.
      const weightset_t& ws_;

      /// Whether to prune states that become inaccessible.
      bool prune_;

      /// dirty states -> proper states.
      std::vector<state_proper_t> d2p_;
      /// proper states -> dirty states.
      std::vector<state_dirty_t> p2d_;

      std::vector<std::vector<weight_t>> de_;
    };

    template <Automaton Aut>
    class epsilon_remover_distance<Aut, false>
    {
      using automaton_t = std::remove_cv_t<Aut>;
      using aut_proper_t = fresh_automaton_t_of<automaton_t>;
    public:
      epsilon_remover_distance(const automaton_t& aut, bool)
        : aut_(aut)
      {}

      aut_proper_t operator()()
      {
        return copy(aut_);
      }

    private:
      automaton_t aut_;
    };
  }
} // namespace vcsn
