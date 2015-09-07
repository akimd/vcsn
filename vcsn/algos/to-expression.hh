#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/info.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/vector.hh>

namespace vcsn
{

  /*----------------.
  | state_chooser.  |
  `----------------*/

  namespace detail
  {
    /*---------------.
    | Naive profile  |
    `---------------*/

    template <typename Aut>
    struct naive_profile
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      naive_profile(state_t state)
        : state_(state)
      {}

      void update(const Aut& a)
      {
        size_t out = 0;
        // Since we are in LAO, there can be at most one such loop.
        // Don't count the loops as out-degree.
        for (auto t: a->all_out(state_))
          if (a->dst_of(t) != state_)
            ++out;
          else
            has_loop_ = true;
        size_t in = a->all_in(state_).size();
        size_ = in * out;
      }

      bool operator<(const naive_profile& rhs) const
      {
        return std::make_tuple(rhs.size_, rhs.has_loop_, rhs.state_)
               < std::make_tuple(size_, has_loop_, state_);
      }

      friend std::ostream& operator<<(std::ostream& o, const naive_profile<Aut>& p)
      {
        return o << p.state_
                 << 's' << p.size_
                 << 'l' << p.has_loop_;
      }

      size_t  size_;
      state_t state_;
      bool    has_loop_ = false;
    };

  /*-------------------.
  | Delgado profile.   |
  `-------------------*/

    template <typename Aut>
    struct delgado_profile // FIXME Call label version
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      delgado_profile(state_t state, bool count_labels = false)
        : state_(state)
        , count_labels_(count_labels)
      {}

      /// The "weight" of a transition.
      ///
      /// That is to say, the size of its expression.
      size_t size_of_transition(transition_t t, const Aut& a) const
      {
        using expset_t = weightset_t_of<automaton_t>;
        if (count_labels_)
          return rat::make_info<expset_t>(a->weight_of(t)).atom;
        else
          return rat::size<expset_t>(a->weight_of(t));
      }


      /// The "weight" of a state, as defined by Degaldo/Morais.
      ///
      /// We use the word "size" instead, since "weight" has already a
      /// strong meaning in Vcsn...
      void update(const Aut& a)
      {
        // The cumulated size of the incoming transitions excluding loops.
        size_t size_in = 0;
        // The number of incoming transitions excluding loops.
        size_t ins = 0;
        // The size of the loop, if there is one.
        size_t size_loop = 0;
        for (auto t: a->all_in(state_))
          if (a->src_of(t) == state_)
            size_loop += size_of_transition(t, a);
          else
            {
              ++ins;
              size_in += size_of_transition(t, a);
            }

        // The cumulated size of the outgoing transitions excluding loops.
        size_t size_out = 0;
        // The number of outgoing transitions excluding loops.
        size_t outs = 0;
        for (auto t: a->all_out(state_))
          if (a->dst_of(t) != state_)
            {
              ++outs;
              size_out += size_of_transition(t, a);
            }

        size_ = (size_in * (outs - 1)
                 + size_out * (ins - 1)
                 + size_loop * (ins * outs - 1));
      }

      bool operator<(const delgado_profile& rhs) const
      {
        return std::make_tuple(rhs.size_, rhs.state_)
               < std::make_tuple(size_, state_);
      }

      friend std::ostream& operator<<(std::ostream& o, const delgado_profile<Aut>& p)
      {
        return o << p.state_
                 << 's' << p.size_;
      }


      size_t  size_;
      state_t state_;
      bool    count_labels_;
    };
  }

  /*------------------.
  | eliminate_state.  |
  `------------------*/

  namespace detail
  {
    template <typename Aut, typename Profile,
             typename Kind = typename context_t_of<Aut>::kind_t>
    struct state_eliminator;

    /// Eliminate states in an automaton whose labelset is oneset.
    template <typename Aut, typename Profile>
    struct state_eliminator<Aut, Profile, labels_are_one>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;
      using profile_t = Profile;

      state_eliminator(automaton_t& aut)
        : aut_(aut)
      {}

      /// Build the profiles and the heap
      void build_heap_()
      {
        for (auto s: aut_->states())
          {
            auto p = profile_t(s);
            p.update(aut_);
            auto h = todo_.emplace(p);
            handles_.emplace(s, h);
          }
      }

      /// Update the profile of \a s.
      void update_profile_(state_t s)
      {
        if (auto p = profile(s))
          p->update(aut_);
      }

      /// Update the heap for \a s.
      /// \pre  its profile is updated.
      void update_heap_(state_t s)
      {
#ifdef DEBUG_UPDATE
        std::cerr << "update heap (" << s << " : ";
        show_heap_();
#endif
        auto i = handles_.find(s);
        if (i != handles_.end())
          todo_.update(i->second);
#ifdef DEBUG_UPDATE
        std::cerr << ") => ";
        show_heap_();
        std::cerr << std::endl;
#endif
      }

      profile_t*
      profile(state_t s)
      {
        auto i = handles_.find(s);
        if (i == handles_.end())
          return nullptr;
        else
          return &*i->second;
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

      /// Eliminate state s.
      void operator()(state_t s)
      {
        if (s == aut_->null_state())
          {
            build_heap_();
            require(!todo_.empty(), "not a valid state: ", s);
            auto p = todo_.top();
            todo_.pop();
            s = p.state_;
          }
        require(aut_->has_state(s), "not a valid state: ", s);
       // The loop's weight.
        auto loop = ws_.zero();
        assert(aut_->outin(s, s).size() <= 1);
        // There is a single possible loop labeled by \e, but it's
        // easier and symmetrical with LAR to use a for-loop.
        for (auto t: make_vector(aut_->outin(s, s)))
          {
            loop = ws_.add(loop, aut_->weight_of(t));
            aut_->del_transition(t);
          }
        loop = ws_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_->all_out(s);
        for (auto in: aut_->all_in(s))
          for (auto out: outs)
            aut_->add_transition
              (aut_->src_of(in), aut_->dst_of(out),
               aut_->label_of(in),
               ws_.mul(aut_->weight_of(in), loop, aut_->weight_of(out)));

        aut_->del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()()
      {
        build_heap_();
        while (!todo_.empty())
          {
#ifdef DEBUG_LOOP
            std::cerr << "Before: ";
            show_heap_();
            std::cerr << std::endl;
#endif

            auto p = todo_.top();
            todo_.pop();

#ifdef DEBUG_LOOP
            std::cerr << "Remove: " << p;
            std::cerr << std::endl;
#endif
            auto s = p.state_;
            handles_.erase(s);

            neighbors_.clear();

            for (auto t: aut_->in(s))
              {
                state_t n = aut_->src_of(t);
                if (n != s)
                  neighbors_.emplace(n);
              }
            for (auto t: aut_->out(s))
              {
                state_t n = aut_->dst_of(t);
                if (n != s)
                  neighbors_.emplace(n);
              }

            operator()(s);

            for (auto n: neighbors_)
              update_profile_(n);
            for (auto n: neighbors_)
              update_heap_(n);
          }
      }

    private:
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();

      /// Max-heap to decide the order of state-elimination.
      using heap_t = boost::heap::fibonacci_heap<profile_t>;
      heap_t todo_;
      /// Map: state -> heap-handle.
      std::unordered_map<state_t, typename heap_t::handle_type> handles_;

      std::unordered_set<state_t> neighbors_;
    };


    /// Eliminate states in an automaton whose labelset is an expressionset.
    template <typename Aut, typename Profile>
    struct state_eliminator<Aut, Profile, labels_are_expressions>
    {
      // FIXME: expressionset<lal_char(a-c), z>, q for instance cannot
      // work, because we need to move the q weights inside the
      // lal_char(a-c), z expressions, which obviously not possible.
      // So we need to require that there is a subtype relationship
      // between the weightset and the weightset of the expression.
      //
      // Yet as of 2014-07, there is no means to check that subtype
      // relationship in Vcsn.

      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using expressionset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;
      using profile_t = Profile;

      state_eliminator(automaton_t& aut)
        : aut_(aut)
      {}

      /// Build the profiles and the heap
      void build_heap_()
      {
        for (auto s: aut_->states())
          {
            auto p = profile_t(s);
            p.update(aut_);
            auto h = todo_.emplace(p);
            handles_.emplace(s, h);
          }
      }

      /// Update the profile of \a s.
      void update_profile_(state_t s)
      {
        if (auto p = profile(s))
          p->update(aut_);
      }

      /// Update the heap for \a s.
      /// \pre  its profile is updated.
      void update_heap_(state_t s)
      {
        auto i = handles_.find(s);
        if (i != handles_.end())
          todo_.update(i->second);
      }

      profile_t*
      profile(state_t s)
      {
        auto i = handles_.find(s);
        if (i == handles_.end())
          return nullptr;
        else
          return &*i->second;
      }

      /// Eliminate state s.
      void operator()(state_t s)
      {
        if (s == aut_->null_state())
          {
            build_heap_();
            require(!todo_.empty(), "not a valid state: ", s);
            auto p = todo_.top();
            todo_.pop();
            s = p.state_;
          }
        require(aut_->has_state(s), "not a valid state: ", s);
        // The loops' expression.
        auto loop = rs_.zero();
        for (auto t: make_vector(aut_->outin(s, s)))
          {
            loop = rs_.add(loop,
                           rs_.lmul(aut_->weight_of(t), aut_->label_of(t)));
            aut_->del_transition(t);
          }
        loop = rs_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_->all_out(s);
        for (auto in: aut_->all_in(s))
          for (auto out: outs)
            aut_->add_transition
              (aut_->src_of(in), aut_->dst_of(out),
              rs_.mul(rs_.lmul(aut_->weight_of(in), aut_->label_of(in)),
                       loop,
                       rs_.lmul(aut_->weight_of(out), aut_->label_of(out))));

        aut_->del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()()
      {
        build_heap_();
        while (!todo_.empty())
          {
            auto p = todo_.top();
            todo_.pop();
            auto s = p.state_;
            handles_.erase(s);

            for (auto t: aut_->in(s))
              {
                state_t n = aut_->src_of(t);
                if (n != s)
                  neighbors_.emplace(n);
              }
            for (auto t: aut_->out(s))
              {
                state_t n = aut_->dst_of(t);
                if (n != s)
                  neighbors_.emplace(n);
              }

            operator()(s);

            for (auto n: neighbors_)
              update_profile_(n);
            for (auto n: neighbors_)
              update_heap_(n);
          }
      }

    private:
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the labelset, which is an expressionset.
      const expressionset_t& rs_ = *aut_->labelset();
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();

      /// Max-heap to decide the order of state-elimination.
      using heap_t = boost::heap::fibonacci_heap<profile_t>;
      heap_t todo_;
      /// Map: state -> heap-handle.
      std::unordered_map<state_t, typename heap_t::handle_type> handles_;

      std::unordered_set<state_t> neighbors_;
    };

    template <typename Aut, typename Profile>
    state_eliminator<Aut, Profile>
    make_state_eliminator(Aut& a)
    {
      return a;
    }
  }


  /// In place removal of state \a s from automaton \a res.
  template <typename Aut>
  Aut&
  eliminate_state_here(Aut& res,
                       state_t_of<Aut> s = Aut::element_type::null_state())
  {
    auto eliminate_state
      = detail::make_state_eliminator<Aut, detail::naive_profile<Aut>>(res);
    eliminate_state(s);
    return res;
  }

  /// A copy of automaton \a res without the state \a s.
  template <typename Aut>
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
      template <typename Aut, typename Int>
      automaton
      eliminate_state(const automaton& aut, int state)
      {
        const auto& a = aut->as<Aut>();
        auto s = 0 <= state ? state + 2 : a->null_state();
        return make_automaton(vcsn::eliminate_state(a, s));
      }
    }
  }


  /*----------------------------.
  | to_expression(automaton).   |
  `----------------------------*/

  template <typename Aut,
            typename Profile,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(Aut& a)
  {
    auto eliminate_states = detail::make_state_eliminator<Aut, Profile>(a);
    eliminate_states();
    return a->get_initial_weight(a->post());
  }

  enum class to_expression_heuristic_t
    {
      best,
      delgado,
      delgado_label,
      naive,
    };

  template <typename Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression_heuristic(const Aut& a, vcsn::rat::identities ids,
                          to_expression_heuristic_t algo)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a, ids);
    switch (algo)
    {
    case to_expression_heuristic_t::best:
      raise("next_state: invalid algorithm: best");

    case to_expression_heuristic_t::delgado:
      return to_expression<decltype(aut),
                           detail::delgado_profile<decltype(aut)>,
                           ExpSet>(aut);

    case to_expression_heuristic_t::delgado_label:
      return to_expression<decltype(aut),
                           detail::delgado_profile<decltype(aut)>,
                           ExpSet>(aut);
      // FIXME Delgado Label

    case to_expression_heuristic_t::naive:
      return to_expression<decltype(aut),
                           detail::naive_profile<decltype(aut)>, ExpSet>(aut);
    }
    BUILTIN_UNREACHABLE();
  }

  template <typename Aut,
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

  template <typename Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(const Aut& a, vcsn::rat::identities ids,
                const std::string& algo)
  {
    static const auto map = std::map<std::string, to_expression_heuristic_t>
      {
        {"auto",          to_expression_heuristic_t::best},
        {"best",          to_expression_heuristic_t::best},
        {"delgado",       to_expression_heuristic_t::delgado},
        {"delgado_label", to_expression_heuristic_t::delgado_label},
        {"naive",         to_expression_heuristic_t::naive},
      };
    return to_expression<Aut, ExpSet>(a, ids, getargs("algorithm", map, algo));
  }

  /*----------------------------.
  | to_expression(automaton).   |
  `----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Identities, typename String>
      expression
      to_expression(const automaton& aut, vcsn::rat::identities ids,
                    const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        using context_t = context_t_of<Aut>;
        using expressionset_t = vcsn::expressionset<context_t>;
        auto rs = expressionset_t{a->context(), ids};
        auto res = ::vcsn::to_expression(a, ids, algo);
        return make_expression(rs, res);
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
      to_expression_label(const context& ctx, rat::identities ids,
                          const label& lbl)
      {
        const auto& c = ctx->as<Context>();
        const auto& l = lbl->as<Label>();
        auto rs = vcsn::make_expressionset(c, ids);
        return make_expression(rs, rs.atom(l.label()));
      }
    }
  }


  /*-------------------------------.
  | to_expression(letter_class).   |
  `-------------------------------*/

  namespace detail
  {
    template <typename ExpSet>
    auto letter_class_impl(const ExpSet&,
                           const letter_class_t&, bool,
                           std::false_type, std::true_type)
      -> typename ExpSet::value_t
    {
      raise("letter_class: not implemented (is_expressionset)");
    }

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

    template <typename ExpSet>
    auto letter_class_impl(const ExpSet& rs,
                           const letter_class_t&, bool,
                           std::true_type, std::false_type)
      -> typename ExpSet::value_t
    {
      return rs.one();
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
      to_expression_class(const context& ctx, rat::identities ids,
                          const letter_class_t& letters, bool accept)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return make_expression(rs, to_expression(rs, letters, accept));
      }
    }
  }
} // vcsn::
