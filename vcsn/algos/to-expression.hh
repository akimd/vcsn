#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/vector.hh>

namespace vcsn
{

  /*----------------.
  | state_chooser.  |
  `----------------*/

  /// A state (inner) from an automaton.
  template <typename Aut,
            typename Lifted = detail::lifted_automaton_tape_t<Aut>>
  using state_chooser_t =
    std::function<state_t_of<Lifted>(const Lifted&)>;


  /*--------------------------.
  | Naive heuristics degree.  |
  `--------------------------*/

  template <typename Aut>
  state_t_of<Aut>
  next_naive(const Aut& a)
  {
    require(!a->states().empty(),
            "state-chooser: empty automaton");

    auto best = a->null_state();
    auto best_has_loop = false;
    auto best_degree = std::numeric_limits<size_t>::max();
    for (auto s: a->states())
      {
        size_t out = 0;
        // Since we are in LAO, there can be at most one such loop.
        bool has_loop = false;
        // Don't count the loops as out-degree.
        for (auto t: a->all_out(s))
          if (a->dst_of(t) != s)
            ++out;
          else
            has_loop = true;
        size_t in = a->all_in(s).size();
        size_t degree = in * out;
        // We prefer to delete a state that has no loop transition.
        if (degree < best_degree
            || (degree == best_degree && has_loop < best_has_loop))
          {
            best = s;
            best_degree = degree;
            best_has_loop = has_loop;
          }
      }
    assert(best != a->null_state());
    return best;
  }

  /*------------------.
  | eliminate_state.  |
  `------------------*/

  namespace detail
  {
    template <typename Aut, typename Kind = typename context_t_of<Aut>::kind_t>
    struct state_eliminator;

    /// Eliminate states in an automaton whose labelset is oneset.
    template <typename Aut>
    struct state_eliminator<Aut, labels_are_one>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;

      state_eliminator(automaton_t& aut)
        : aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        if (s == aut_->null_state())
          s = next_naive(aut_);
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
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_->num_states())
          operator()(next_state(aut_));
      }

    private:
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();
    };


    /// Eliminate states in an automaton whose labelset is an expressionset.
    template <typename Aut>
    struct state_eliminator<Aut, labels_are_expressions>
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

      state_eliminator(automaton_t& aut)
        : aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        if (s == aut_->null_state())
          s = next_naive(aut_);
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
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_->num_states())
          operator()(next_state(aut_));
      }

    private:
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the labelset, which is an expressionset.
      const expressionset_t& rs_ = *aut_->labelset();
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();
    };

    template <typename Aut>
    state_eliminator<Aut>
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
    auto eliminate_state = detail::make_state_eliminator(res);
    eliminate_state(s);
    return res;
  }

  /// A copy of automaton \a res without the state \a s.
  template <typename Aut>
  Aut
  eliminate_state(const Aut& aut,
                  state_t_of<Aut> s = Aut::element_type::null_state())
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
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression(const Aut& a, vcsn::rat::identities ids,
                const state_chooser_t<Aut>& next_state)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a, ids);
    auto eliminate_states = detail::make_state_eliminator(aut);
    eliminate_states(next_state);
    return aut->get_initial_weight(aut->post());
  }


  template <typename Aut,
            typename ExpSet = expressionset<context_t_of<Aut>>>
  typename ExpSet::value_t
  to_expression_naive(const Aut& a, vcsn::rat::identities ids)
  {
    state_chooser_t<Aut> next
      = next_naive<detail::lifted_automaton_tape_t<Aut>>;
    return to_expression(a, ids, next);
  }

  /*----------------------.
  | dyn::to_expression.   |
  `----------------------*/

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
        // FIXME: So far, there is a single implementation of expressions,
        // but we should actually be parameterized by its type too.
        using context_t = context_t_of<Aut>;
        using expressionset_t = vcsn::expressionset<context_t>;
        auto rs = expressionset_t{a->context(), ids};
        static const auto map = std::map<std::string, bool>
          {
            {"auto",  true},
            {"naive", true},
          };
        // Merely just a validation so far.
        getargs("algorithm", map, algo);
        return make_expression(rs, ::vcsn::to_expression_naive(a, ids));
      }

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
      using word_t = typename labelset_t::word_t;
      using label_t = typename labelset_t::value_t;
      using letter_t = typename labelset_t::letter_t;

      auto ccs = std::set<std::pair<letter_t, letter_t>>{};
      for (auto cc: chars)
        {
          // Yes, this is ugly: to convert an std::string into a
          // letter_t, we (i) parse the string into a label, (ii)
          // convert it into a word_t, (iii) from which we extract its
          // first letter.
          label_t lbl1 = ::vcsn::conv(ls, cc.first);
          label_t lbl2 = ::vcsn::conv(ls, cc.second);
          word_t w1 = ls.word(lbl1);
          word_t w2 = ls.word(lbl2);
          letter_t l1 = *std::begin(ls.letters_of(w1));
          letter_t l2 = *std::begin(ls.letters_of(w2));
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
