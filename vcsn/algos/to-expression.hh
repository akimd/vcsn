#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/core/automaton.hh> // all_in
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/info.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/misc/fibonacci_heap.hh>

namespace vcsn
{
  namespace detail
  {
    /*----------------.
    | Naive profiler. |
    `----------------*/

    /// Compute a state profile for state-elimination based on
    /// connectivity.
    template <Automaton Aut>
    struct naive_profiler
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      naive_profiler(const automaton_t& aut)
        : aut_(aut)
      {}

      /// The state profile is the product of the number of (strictly)
      /// incoming transitions with the number of (strictly) outgoing
      /// ones, and whether it has loops.
      struct state_profile
      {
        state_profile(state_t state)
          : state_(state)
        {}

        bool operator<(const state_profile& rhs) const
        {
          return std::make_tuple(size_, has_loop_, state_)
                 < std::make_tuple(rhs.size_, rhs.has_loop_, rhs.state_);
        }

        friend std::ostream& operator<<(std::ostream& o, const state_profile& p)
        {
          return o << p.state_
                   << 's' << p.size_
                   << 'l' << p.has_loop_;
        }

        /// Number of strictly incoming transitions, times the number
        /// of strictly outgoing transitions.
        size_t size_ = 0;
        bool has_loop_ = false;
        state_t state_;
      };

      state_profile
      make_state_profile(state_t state)
      {
        auto p = state_profile{state};
        update(p);
        return p;
      }

      void invalidate_cache(transition_t)
      {}

      void update(state_profile& p)
      {
        size_t out = 0;
        for (auto t: all_out(aut_, p.state_))
          if (aut_->dst_of(t) == p.state_)
            p.has_loop_ = true;
          else
            ++out;
        size_t in = all_in(aut_, p.state_).size();
        p.size_ = in * out;
      }

      automaton_t aut_;
    };

    /*-------------------.
    | Delgado profiler.  |
    `-------------------*/

    /// Compute a state profile for state-elimination based on the
    /// Delgado-Morais heuristic.
    template <Automaton Aut>
    struct delgado_profiler
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      /// Build a generator of Delgado-Morais state profiles.
      ///
      /// \param aut  the input automaton
      /// \param count_labels  whether we use the width of the expressions,
      ///                      instead of the length.
      delgado_profiler(const automaton_t& aut, bool count_labels = false)
        : aut_{aut}
        , count_labels_{count_labels}
        , transition_cache_(transitions_size(aut_), 0)
      {}

      struct state_profile
      {
        state_profile(state_t state)
          : state_(state)
        {}

        bool operator<(const state_profile& rhs) const
        {
          return std::make_tuple(size_, state_)
                 < std::make_tuple(rhs.size_, rhs.state_);
        }

        friend std::ostream& operator<<(std::ostream& o, const state_profile& p)
        {
          return o << p.state_
                   << 's' << p.size_;
        }

        size_t size_ = 0;
        state_t state_;
      };

      state_profile
      make_state_profile(state_t state)
      {
        auto p = state_profile{state};
        update(p);
        return p;
      }

      /// The "weight" of a transition.
      ///
      /// That is to say, the size of its expression.
      size_t size_of_transition(transition_t t)
      {
        auto& res = transition_cache_[t];
        if (res == 0)
          {
            using expset_t = weightset_t_of<automaton_t>;
            if (count_labels_)
              res = rat::make_info<expset_t>(aut_->weight_of(t)).atom;
            else
              res = rat::size<expset_t>(aut_->weight_of(t));
          }
        return res;
      }

      /// Updating transitions' size in the cache during the profiler's
      /// construction would be clearer but appear to be less efficient.
      /// Invalidate Update
      /// 0.29s      0.37s : a.expression(linear, delgado) # a = std([a-d]?{15})
      void invalidate_cache(transition_t t)
      {
        if (transition_cache_.size() <= t)
          transition_cache_.resize(t + 1, 0);
        transition_cache_[t] = 0;
      }

      /// The "weight" of a state, as defined by Delgado-Morais.
      ///
      /// We use the word "size" instead, since "weight" has already a
      /// strong meaning in Vcsn...
      void update(state_profile& p)
      {
        // The cumulated size of the incoming transitions excluding loops.
        size_t size_in = 0;
        // The number of incoming transitions excluding loops.
        size_t ins = 0;
        // The size of the loop, if there is one.
        size_t size_loop = 0;
        for (auto t: all_in(aut_, p.state_))
          if (aut_->src_of(t) == p.state_)
            size_loop += size_of_transition(t);
          else
            {
              ++ins;
              size_in += size_of_transition(t);
            }

        // The cumulated size of the outgoing transitions excluding loops.
        size_t size_out = 0;
        // The number of outgoing transitions excluding loops.
        size_t outs = 0;
        for (auto t: all_out(aut_, p.state_))
          if (aut_->dst_of(t) != p.state_)
            {
              ++outs;
              size_out += size_of_transition(t);
            }

        p.size_ = (size_in * (outs - 1)
                   + size_out * (ins - 1)
                   + size_loop * (ins * outs - 1));
      }

      automaton_t aut_;
      bool count_labels_;
      std::vector<size_t> transition_cache_;
    };
  }

  /*------------------.
  | eliminate_state.  |
  `------------------*/

  namespace detail
  {
    /// Eliminate states in an automaton.
    ///
    /// \pre The labelset is oneset or expressionset.
    template <Automaton Aut, typename Profiler>
    struct state_eliminator
    {
      using profiler_t = Profiler;
      using profile_t = typename profiler_t::state_profile;
      using automaton_t = std::remove_cv_t<Aut>;
      using state_t = state_t_of<automaton_t>;

      /// Prepare for state-elimination.
      ///
      /// \param aut       the input automaton
      /// \param profiler  the generator of state profiles.
      state_eliminator(automaton_t& aut, profiler_t& profiler)
        : aut_(aut)
        , profiler_(profiler)
        , handles_(states_size(aut_))
      {
        for (auto s: aut_->states())
          handles_[s] = todo_.emplace(profiler_.make_state_profile(s));
      }

      /// Eliminate state s.
      void operator()(state_t s)
      {
        if (s == aut_->null_state())
          {
            require(!todo_.empty(), "no state left to remove");
            auto p = todo_.top();
            todo_.pop();
            s = p.state_;
          }
        else
          require(aut_->has_state(s), "not a valid state: ", s);
        eliminate_state_(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()()
      {
        while (!todo_.empty())
          {
            auto p = todo_.top();
            todo_.pop();

#ifdef DEBUG_LOOP
            std::cerr << "Remove: " << p << '\n';
#endif
            eliminate_state_(p.state_);
          }
      }

    private:
      void update_heap_()
      {
#ifdef DEBUG_UPDATE
        std::cerr << "update heap: (";
        show_heap_();
#endif
        for (auto s: neighbors_)
          if (s != aut_->pre() && s != aut_->post())
            {
              auto& h = handles_[s];
              profiler_.update(*h);
              todo_.update(h);
            }
#ifdef DEBUG_UPDATE
        std::cerr << ") => ";
        show_heap_();
        std::cerr << '\n';
#endif
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

      /// Eliminate state s in the case of labels are one.
      template <typename Kind = typename context_t_of<automaton_t>::kind_t>
      auto eliminate_state_(state_t s)
        -> std::enable_if_t<std::is_same<Kind, labels_are_one>::value,
                       void>
      {
        neighbors_.clear();

        // Shorthand to the labelset.
        const auto& ls = *aut_->labelset();

        // Shorthand to the weightset.
        const auto& ws = *aut_->weightset();

        // The loop's weight.
        auto loop = ws.zero();
        assert(outin(aut_, s, s).size() <= 1);
        // There is a single possible loop labeled by \e, but it's
        // easier and symmetrical with LAR to use a for-loop.
        for (auto t: make_vector(outin(aut_, s, s)))
          {
            loop = ws.add(loop, aut_->weight_of(t));
            aut_->del_transition(t);
          }
        loop = ws.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = all_out(aut_, s);
        for (auto in: all_in(aut_, s))
          for (auto out: outs)
            {
              auto src = aut_->src_of(in);
              auto dst = aut_->dst_of(out);
              auto t = aut_->add_transition
                (src, dst,
                 // Of course, most of the time \e\e => \e.  But there
                 // is also $ to take into account when eliminating an
                 // initial/final state.
                 ls.mul(aut_->label_of(in), aut_->label_of(out)),
                 ws.mul(aut_->weight_of(in), loop, aut_->weight_of(out)));
              profiler_.invalidate_cache(t);
              neighbors_.emplace(src);
              neighbors_.emplace(dst);
            }

        aut_->del_state(s);
        update_heap_();
      }

      /// Eliminate state s in the case of labels are expressions.
      //
      // FIXME: expressionset<lal_char(a-c), z>, q for instance cannot
      // work, because we need to move the q weights inside the
      // lal_char(a-c), z expressions, which is obviously not possible.
      // So we need to require that there is a subtype relationship
      // between the weightset and the weightset of the expression.
      //
      // Yet as of 2014-07, there is no means to check that subtype
      // relationship in Vcsn.
      template <typename Kind>
      auto eliminate_state_impl_(state_t s)
        -> std::enable_if_t<std::is_same<Kind, labels_are_expressions>::value,
                       void>
      {
        neighbors_.clear();

        // Shorthand to the labelset, which is an expressionset.
        const auto& rs = *aut_->labelset();

        // The loops' expression.
        auto loop = rs.zero();
        for (auto t: make_vector(outin(aut_, s, s)))
          {
            loop = rs.add(loop,
                          rs.lweight(aut_->weight_of(t), aut_->label_of(t)));
            aut_->del_transition(t);
          }
        loop = rs.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = all_out(aut_, s);
        for (auto in: all_in(aut_, s))
          for (auto out: outs)
            {
              auto src = aut_->src_of(in);
              auto dst = aut_->dst_of(out);
              auto t = aut_->add_transition
                (src, dst,
                 rs.mul(rs.lweight(aut_->weight_of(in), aut_->label_of(in)),
                        loop,
                        rs.lweight(aut_->weight_of(out), aut_->label_of(out))));
              profiler_.invalidate_cache(t);
              neighbors_.emplace(src);
              neighbors_.emplace(dst);
            }

        aut_->del_state(s);
        update_heap_();
      }

      /// The automaton we work on.
      automaton_t aut_;
      /// The profiler we work with. Corresponding to a specific heuristic.
      profiler_t& profiler_;

      /// Max-heap to decide the order of state-elimination.
      using heap_t = vcsn::min_fibonacci_heap<profile_t>;
      heap_t todo_;
      /// Map: state -> heap-handle.
      std::vector<typename heap_t::handle_type> handles_;

      std::unordered_set<state_t> neighbors_;
    };

    template <Automaton Aut, typename Profiler>
    state_eliminator<Aut, Profiler>
    make_state_eliminator(Aut& a, Profiler& profiler)
    {
      return {a, profiler};
    }
  }


  /// In place removal of state \a s from automaton \a res.
  template <Automaton Aut>
  Aut&
  eliminate_state_here(Aut& res,
                       state_t_of<Aut> s = Aut::element_type::null_state())
  {
    // Delgado profiler not fit for lao with non expression weightset.
    auto profiler = detail::naive_profiler<Aut>(res);
    auto eliminate_state = detail::make_state_eliminator(res, profiler);
    eliminate_state(s);
    return res;
  }

  /// A copy of automaton \a res without the state \a s.
  template <Automaton Aut>
  auto
  eliminate_state(const Aut& aut,
                  state_t_of<Aut> s = Aut::element_type::null_state())
    -> fresh_automaton_t_of<Aut>
  {
    // Get a copy, but be sure to keep the correspondance bw original
    // state numbers and the new ones.
    auto res = make_fresh_automaton<Aut>(aut);
    auto copy = make_copier(aut, res);
    copy();
    if (s != aut->null_state())
      {
        require(aut->has_state(s), "not a valid state: ", s);
        s = copy.state_map().at(s);
      }
    return eliminate_state_here(res, s);
  }


  /*-----------------------.
  | dyn::eliminate_state.  |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Int>
      automaton
      eliminate_state(const automaton& aut, int state)
      {
        const auto& a = aut->as<Aut>();
        using state_t = state_t_of<Aut>;
        auto s = 0 <= state ? state_t(state + 2) : a->null_state();
        return vcsn::eliminate_state(a, s);
      }
    }
  }


  /*----------------------------.
  | to_expression(automaton).   |
  `----------------------------*/

  template <Automaton Aut,
            typename Profiler,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(Aut& a, Profiler& profiler)
  {
    try
      {
        auto eliminate_states = detail::make_state_eliminator(a, profiler);
        eliminate_states();
        return a->get_initial_weight(a->post());
      }
    catch (const std::runtime_error& e)
      {
        raise(e, "  while making expression");
      }
  }

  enum class to_expression_heuristic_t
    {
      best,
      delgado,
      delgado_label,
      naive,
    };

  template <Automaton Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression_heuristic(const Aut& aut, vcsn::rat::identities ids,
                          to_expression_heuristic_t algo)
  {
    // State elimination is performed on the lifted automaton.
    auto a = lift(aut, ids);
    using delgado_t = detail::delgado_profiler<decltype(a)>;
    using naive_t = detail::naive_profiler<decltype(a)>;
    switch (algo)
    {
    case to_expression_heuristic_t::best:
      raise("next_state: invalid algorithm: best");

    case to_expression_heuristic_t::delgado:
      {
        auto profiler = delgado_t(a);
        return to_expression<decltype(a), delgado_t, ExpSet>(a, profiler);
      }

    case to_expression_heuristic_t::delgado_label:
      {
        auto profiler = delgado_t(a, true);
        return to_expression<decltype(a), delgado_t, ExpSet>(a, profiler);
      }

    case to_expression_heuristic_t::naive:
      {
        auto profiler = naive_t(a);
        return to_expression<decltype(a), naive_t, ExpSet>(a, profiler);
      }
    }
    BUILTIN_UNREACHABLE();
  }

  template <Automaton Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(const Aut& aut, vcsn::rat::identities ids,
                to_expression_heuristic_t algo)
  {
    if (algo == to_expression_heuristic_t::best)
      {
        typename ExpSet::value_t best;
        auto best_size = std::numeric_limits<size_t>::max();
        for (auto a: {to_expression_heuristic_t::delgado,
                      to_expression_heuristic_t::delgado_label,
                      to_expression_heuristic_t::naive})
          {
            auto r = to_expression_heuristic<Aut, ExpSet>(aut, ids, a);
            auto s = rat::size<ExpSet>(r);
            if (s < best_size)
              {
                best = r;
                best_size = s;
              }
          }
        return best;
      }
    else
      {
        return to_expression_heuristic<Aut, ExpSet>(aut, ids, algo);
      }
  }

  template <Automaton Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(const Aut& a, vcsn::rat::identities ids,
                const std::string& algo)
  {
    static const auto map = getarg<to_expression_heuristic_t>
      {
        "heuristics",
        {
          {"auto",          "best"},
          {"best",          to_expression_heuristic_t::best},
          {"delgado",       to_expression_heuristic_t::delgado},
          {"delgado_label", to_expression_heuristic_t::delgado_label},
          {"naive",         to_expression_heuristic_t::naive}
        }
      };
    return to_expression<Aut, ExpSet>(a, ids, map[algo]);
  }

  /*----------------------------.
  | to_expression(automaton).   |
  `----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Identities, typename String>
      expression
      to_expression(const automaton& aut, identities ids,
                    const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        using context_t = context_t_of<Aut>;
        using expressionset_t = vcsn::expressionset<context_t>;
        auto rs = expressionset_t{a->context(), ids};
        auto res = ::vcsn::to_expression(a, ids, algo);
        return {rs, res};
      }
    }
  }

  /*------------------------.
  | to_expression(label).   |
  `------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (to_expression).
      template <typename Context, typename Identities, typename Label>
      expression
      to_expression_label(const context& ctx, identities ids,
                          const label& lbl)
      {
        const auto& c = ctx->as<Context>();
        const auto& l = lbl->as<Label>();
        auto rs = vcsn::make_expressionset(c, ids);
        return {rs, rs.atom(l.value())};
      }
    }
  }


  /*-------------------------------.
  | to_expression(letter_class).   |
  `-------------------------------*/

  namespace detail
  {
    /// Case where labelset is an expressionset.
    template <typename ExpSet>
    auto letter_class_impl(const ExpSet&,
                           const letter_class_t&, bool,
                           std::false_type, std::true_type)
      -> typename ExpSet::value_t
    {
      raise("letter_class: not implemented (is_expressionset)");
    }

    /// Case where labelset is oneset.
    template <typename ExpSet>
    auto letter_class_impl(const ExpSet& rs,
                           const letter_class_t&, bool,
                           std::true_type, std::false_type)
      -> typename ExpSet::value_t
    {
      return rs.one();
    }

    /// General case.
    template <typename ExpSet>
    auto letter_class_impl(const ExpSet& rs,
                           const letter_class_t& chars, bool accept,
                           std::false_type, std::false_type)
      -> typename ExpSet::value_t
    {
      auto ls = *rs.labelset();

      using labelset_t = decltype(ls);
      using letter_t = typename labelset_t::letter_t;

      auto ccs = std::set<std::pair<letter_t, letter_t>>{};
      for (const auto& cc: chars)
        {
          std::istringstream i1{cc.first};
          std::istringstream i2{cc.second};
          letter_t l1 = ls.get_letter(i1, false);
          letter_t l2 = ls.get_letter(i2, false);
          ccs.emplace(l1, l2);
        }
      return rs.letter_class(ccs, accept);
    }
  }

  /// An expression matching one letter in a letter class.
  ///
  /// \param rs
  ///   The expressionset to use.
  /// \param letters
  ///   The letter class as a set of ranges.
  /// \param accept
  ///   Whether to accept these characters ([abc]) as opposed
  ///   to refusing them ([^abc]).
  template <typename ExpressionSet>
  typename ExpressionSet::value_t
  to_expression(const ExpressionSet& rs, const letter_class_t& letters,
                bool accept = true)
  {
    using labelset_t = labelset_t_of<ExpressionSet>;
    using is_one_t = std::is_same<labelset_t, vcsn::oneset>;
    using is_expset_t = bool_constant<labelset_t::is_expressionset()>;
    return detail::letter_class_impl(rs, letters, accept,
                                     is_one_t{}, is_expset_t{});
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (to_expression).
      template <typename Context, typename Identities,
                typename Letters, typename Bool>
      expression
      to_expression_class(const context& ctx, identities ids,
                          const letter_class_t& letters, bool accept)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return {rs, to_expression(rs, letters, accept)};
      }
    }
  }
} // vcsn::
