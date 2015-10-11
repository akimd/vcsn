#pragma once

#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/dot.hh>
#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/debug-level.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/epsilon-profile.hh>

#define STATS

namespace vcsn
{
  namespace detail
  {
    /// This class contains the core of the proper algorithm.
    ///
    /// This class is specialized for labels_are_letter automata since
    /// all these methods become trivial.
    template <typename Aut,
              bool has_one = labelset_t_of<Aut>::has_one()>
    class epsilon_remover_separate
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using labelset_t = labelset_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using transitions_t = std::vector<transition_t>;

      using proper_ctx_t = detail::proper_context<context_t_of<Aut>>;
      using dirty_ctx_t = context<vcsn::oneset, weightset_t>;

      using aut_dirty_t = mutable_automaton<dirty_ctx_t>;
      using aut_proper_t = fresh_automaton_t_of<Aut, proper_ctx_t>;

      using label_proper_t = label_t_of<aut_proper_t>;

    public:
      /// Get ready to eliminate spontaneous transitions.
      /// \param aut    the automaton in which to remove them
      /// \param prune  whether to delete states that become inaccessible
      epsilon_remover_separate(const automaton_t& aut, bool prune = true)
        : debug_(debug_level())
        , ws_(*aut->weightset())
        , prune_(prune)
        , d2p_(aut->all_states().back() + 1, aut->null_state())
        , p2d_(aut->all_states().back() + 1, aut->null_state())
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
                  return (!aut->in(s, aut->labelset()->one()).empty()
                          || !aut->out(s, aut->labelset()->one()).empty());
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
      }

    private:

      /// Update the profile of \a s.
      void update_profile_(state_t proper_s)
      {
        state_t dirty_s = p2d_[proper_s];
        if (auto p = profile(proper_s))
          {
            auto in_dirty = aut_dirty_->in(dirty_s).size();
            auto in_proper = aut_proper_->in(proper_s).size();
            auto out_dirty = aut_dirty_->out(dirty_s).size();
            auto out_proper = aut_proper_->all_out(proper_s).size();

            p->update(in_dirty, in_proper + in_dirty,
                      out_dirty, out_proper + out_dirty);
          }
      }

      /// Build the profiles and the heap for states with incoming
      /// spontaneous transitions.
      void build_heap_()
      {
        for (auto s: aut_dirty_->states())
          // We don't care about states without incoming spontaneous
          // transitions.
          if (aut_dirty_->in(s).size())
            {
              auto proper_s = d2p_[s];
              auto h = todo_.emplace(epsilon_profile<state_t>
                                     {proper_s, 0, 0, 0, 0});
              handles_.emplace(proper_s, h);
              update_profile_(proper_s);
            }
      }

      /// Show the heap, for debugging.
      void show_heap_() const
      {
        const char* sep = "";
        for (auto i = todo_.ordered_begin(), end = todo_.ordered_end();
             i != end; ++i)
          {
            std::cerr << sep << *i;
            sep = " > ";
          }
      }

      /// Update the heap for \a s.
      /// \pre  its profile is updated.
      void update_heap_(state_t s)
      {
        if (3 < debug_)
          {
            std::cerr << "update heap (" << s << " : ";
            show_heap_();
          }
        auto i = handles_.find(s);
        if (i != handles_.end())
          todo_.update(i->second);
        if (3 < debug_)
          {
            std::cerr << ") => ";
            show_heap_();
            std::cerr << std::endl;
          }
      }

#ifdef STATS
      unsigned added_ = 0;
      unsigned removed_ = 0;
#endif

      epsilon_profile<state_t>*
      profile(state_t s)
      {
        auto i = handles_.find(s);
        if (i == handles_.end())
          return nullptr;
        else
          return &*i->second;
      }

      /// For each state (s), for each incoming epsilon-transitions
      /// (t), if (t) is a loop, the star of its weight is computed,
      /// otherwise, (t) is stored into the closure list.  Then (t) is
      /// removed.
      ///
      /// Because it is sometimes useful to be able to invoke proper
      /// on a single state, we kept this function free from any
      /// relation with the profiles and the heap.
      ///
      /// For some reason, we get poorer performances if this function
      /// is moved higher, before the epsilon_profile definition.
      ///
      /// The state s corresponds to a *dirty* state id.
      void remover_on(state_t dirty_s, state_t proper_s)
      {
        const auto& tr = aut_dirty_->in(dirty_s);
        // Iterate on a copy, as we remove these transitions in the
        // loop.
        transitions_t transitions{tr.begin(), tr.end()};
        // The star of the weight of the loop on 's' (1 if no loop).
        weight_t star = ws_.one();
        using state_weight_t = std::pair<state_t, weight_t>;
        std::vector<state_weight_t> closure;
        for (auto t : transitions)
          {
            weight_t weight = aut_dirty_->weight_of(t);
            state_t src = aut_dirty_->src_of(t);
            if (src == dirty_s)  //loop
              star = ws_.star(weight);
            else
              closure.emplace_back(src, weight);
            // Delete incoming epsilon transitions.
            aut_dirty_->del_transition(t);
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


        // TODO: factoring with lambda
        for (auto t: aut_dirty_->out(dirty_s))
          {
            weight_t blow = ws_.mul(star, aut_dirty_->weight_of(t));
            aut_dirty_->set_weight(t, blow);

            state_t dst = aut_dirty_->dst_of(t);
            for (auto pair: closure)
              {
                state_t src = pair.first;
                weight_t w = ws_.mul(pair.second, blow);
                aut_dirty_->add_transition(src, dst, {}, w);
              }
          }

        for (auto t: aut_proper_->all_out(proper_s))
          {
            weight_t blow = ws_.mul(star, aut_proper_->weight_of(t));
            aut_proper_->set_weight(t, blow);

            label_proper_t label = aut_proper_->label_of(t);
            state_t dst = aut_proper_->dst_of(t);
            for (auto pair: closure)
              {
                state_t src = pair.first;
                weight_t w = ws_.mul(pair.second, blow);
                aut_proper_->add_transition(d2p_[src], dst, label, w);
              }
          }
#ifdef STATS
        unsigned added = (aut_proper_->all_out(proper_s).size()
                          + aut_dirty_->out(dirty_s).size()) * closure.size();
        unsigned removed = transitions.size();
#endif
        if (prune_ &&
            aut_dirty_->in(dirty_s).empty() &&
            aut_proper_->all_in(proper_s).empty())
          {
#ifdef STATS
            removed += (aut_proper_->all_out(proper_s).size()
                        + aut_dirty_->out(dirty_s).size());
#endif
            aut_proper_->del_state(proper_s);
            aut_dirty_->del_state(dirty_s);
          }
#ifdef STATS
        added_ += added;
        removed_ += removed;
        if (1 < debug_)
          std::cerr << " -" << removed << "+" << added
                    << " (-" << removed_ << "+" << added_ << ")";
#endif
      }

    public:

      /// Remove all the states with incoming spontaneous transitions.
      ///
      /// Set-up and maintain a heap to process states in an order
      /// that attempts to avoid useless introducing useless
      /// spontaneous transitions.
      ///
      /// Think for instance of the applying proper to
      /// thompson(a?{n}): it is much more efficient to work "from
      /// final to initial states", than the converse (which is what
      /// the "implementation order" actually does).  For n=2000, the
      /// "implementation order" takes 102s on my machine, while this
      /// order (and its implementation) takes 15.2s.
      aut_proper_t operator()()
      {
        if (4 < debug_)
          {
            dot(aut_proper_, std::cerr) << std::endl;
            dot(aut_dirty_, std::cerr) << std::endl;
          }
        build_heap_();
        /* For each state (s), for each incoming epsilon-transitions
           (t), if (t) is a loop, the star of its weight is computed,
           otherwise, (t) is stored into the closure list.  Then (t)
           is removed.  */

        // The neighbors of s: their profiles need to be updated after
        // s was processed.
        std::unordered_set<state_t> neighbors;
        while (!todo_.empty())
          {
            if (2 < debug_)
              {
                std::cerr << "Before: ";
                show_heap_();
                std::cerr << std::endl;
              }
            auto p = todo_.top();
            todo_.pop();
            if (1 < debug_)
              std::cerr << "Remove: " << p;

            auto proper_s = p.state;
            auto dirty_s = p2d_[proper_s];
            handles_.erase(proper_s);
            neighbors.clear();
            state_t n;
            for (auto t: aut_proper_->in(proper_s))
                if ((n = aut_proper_->src_of(t)) != proper_s)
                  neighbors.emplace(n);
            for (auto t: aut_proper_->out(proper_s))
                if ((n = aut_proper_->dst_of(t)) != proper_s)
                  neighbors.emplace(n);

            if (dirty_s != aut_dirty_->null_state())
              {
                for (auto t: aut_dirty_->in(dirty_s))
                    if ((n = aut_dirty_->src_of(t)) != dirty_s)
                      neighbors.emplace(d2p_[n]);
                for (auto t: aut_dirty_->out(dirty_s))
                    if ((n = aut_dirty_->dst_of(t)) != dirty_s)
                      neighbors.emplace(d2p_[n]);

                remover_on(dirty_s, proper_s);
              }

            // Update all neighbors and then the heap.
            for (auto n: neighbors)
              update_profile_(n);
            for (auto n: neighbors)
              update_heap_(n);
            if (1 < debug_)
              std::cerr << " #tr: "
                        << aut_dirty_->transitions().size()
                        << "/" << (aut_dirty_->transitions().size()
                                   + aut_proper_->transitions().size())
                        << std::endl;
            if (2 < debug_)
              {
                std::cerr << "After:  ";
                show_heap_();
                std::cerr << std::endl;
              }
            if (4 < debug_)
              {
                dot(aut_proper_, std::cerr) << std::endl;
                dot(aut_dirty_, std::cerr) << std::endl;
              }
            if (2 < debug_)
              std::cerr << std::endl;
          }
#ifdef STATS
        if (0 < debug_)
          std::cerr << "Total transitions -" << removed_
                    << "+" << added_ << std::endl;
#endif

        return aut_proper_;
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;

      /// The automata we work on.
      aut_proper_t aut_proper_;
      aut_dirty_t aut_dirty_;

      /// Shorthand to the weightset.
      const weightset_t& ws_;

      /// Max-heap to decide the order of state-elimination.
      using heap_t = boost::heap::fibonacci_heap<epsilon_profile<state_t>>;
      heap_t todo_;
      /// Map: state -> heap-handle.
      std::unordered_map<state_t, typename heap_t::handle_type> handles_;

      /// Whether to prune states that become inaccessible.
      bool prune_;

      std::vector<state_t> d2p_; // dirty states -> proper states
      std::vector<state_t> p2d_; // proper states -> dirty states
    };

    template <typename Aut>
    class epsilon_remover_separate<Aut, false>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using aut_proper_t = fresh_automaton_t_of<automaton_t>;
    public:
      epsilon_remover_separate(const automaton_t& aut, bool)
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
