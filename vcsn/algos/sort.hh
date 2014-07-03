#ifndef VCSN_ALGOS_SORT_HH
# define VCSN_ALGOS_SORT_HH

# include <map>
# include <queue>
# include <vector>

# include <vcsn/core/permutation-automaton.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/algorithm.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/unordered_map.hh>

# include <vcsn/algos/copy.hh> // real_context
# include <vcsn/ctx/traits.hh> // base_t

namespace vcsn
{


  /*------------------.
  | is_label_sorted.  |
  `------------------*/

  /// Whether for each state, the outgoing transitions are sorted by
  /// increasing label.
  template <typename Aut>
  inline
  bool
  is_out_sorted(const Aut& a)
  {
    using transition_t = transition_t_of<Aut>;
    for (state_t_of<Aut> s: a->states())
      if (!detail::is_sorted(a->out(s),
                             [&a] (transition_t l, transition_t r)
                             {
                               return a->labelset()->less_than(a->label_of(l),
                                                              a->label_of(r));
                             }))
        return false;
    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool
      is_out_sorted(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_out_sorted(a);
      }

      REGISTER_DECLARE(is_out_sorted,
                       (const automaton&) -> bool);
    }
  }


  /*-------.
  | sort.  |
  `-------*/
  namespace detail
  {

    template <typename Aut>
    class sorter
    {
      /// Input automaton type.
      using input_automaton_t = Aut;

      using input_state_t = state_t_of<input_automaton_t>;
      using input_transition_t = transition_t_of<input_automaton_t>;

      /// Result automaton type.
      using automaton_t = permutation_automaton<input_automaton_t>;
      using state_t = state_t_of<automaton_t>;

      void visit_successors_of(input_state_t s, state_t res_s)
      {
        std::vector<input_transition_t> ts;
        // Here a_->out(s) would just as well as a_->all_out(s) but it
        // would be slower; later we have to test one condition per
        // transition anyway, which is just the additional work
        // performed by out.
        for (auto t: res_->input_->all_out(s))
          ts.emplace_back(t);

        std::sort(ts.begin(), ts.end(),
                  [&](const input_transition_t t1,
                      const input_transition_t t2) -> bool
                  {
                    return transition_less_than(t1, t2);
                  });

        for (auto t: ts)
          res_->new_transition_copy(res_s, res_->state(res_->input_->dst_of(t)),
                                    res_->input_, t);
      }

      void visit_and_update_res()
      {
        while (! res_->todo_.empty())
          {
            auto p = res_->todo_.front();
            res_->todo_.pop();
            visit_successors_of(p.first, p.second);
          }
      }

      void push_inaccessible_states()
      {
        // States are processed in order.  Like above, a_->states()
        // would work.
        for (auto s: res_->input_->all_states())
          res_->state(s);
      }

      bool transition_less_than(const input_transition_t t1,
                                const input_transition_t t2) const
        ATTRIBUTE_PURE
      {
        // We intentionally ignore source states: they should always
        // be identical when we call this.
        auto& aut = res_->input_;
        assert(aut->src_of(t1) == aut->src_of(t2));
        if (ls_.less_than(aut->label_of(t1), aut->label_of(t2)))
          return true;
        else if (ls_.less_than(aut->label_of(t2), aut->label_of(t1)))
          return false;
        else if (ws_.less_than(aut->weight_of(t1), aut->weight_of(t2)))
          return true;
        else if (ws_.less_than(aut->weight_of(t2), aut->weight_of(t1)))
          return false;
        else if (aut->dst_of(t1) < aut->dst_of(t2))
          return true;
        else if (aut->dst_of(t2) < aut->dst_of(t1))
          return false;
        else
          return false;
      }

    public:
      sorter(const input_automaton_t& a)
        : res_(make_shared_ptr<automaton_t>(a))
      {}

      automaton_t operator()()
      {
        visit_and_update_res();
        push_inaccessible_states();
        visit_and_update_res();
        return std::move(res_);
      }

    private:
      /// Sorted automaton.
      automaton_t res_;
      const labelset_t_of<input_automaton_t>& ls_ = *res_->input_->labelset();
      const weightset_t_of<input_automaton_t>& ws_ = *res_->input_->weightset();
    }; // class
  } // namespace

  template <typename Aut>
  inline
  auto
  sort(const Aut& a)
    -> permutation_automaton<Aut>
  {
    detail::sorter<Aut> sorter(a);
    return sorter();
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Aut>
      automaton
      sort(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::sort(a));
      }

      REGISTER_DECLARE(sort,
                       (const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_SORT_HH
