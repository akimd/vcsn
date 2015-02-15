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
    auto less = [&a] (transition_t l, transition_t r)
      {
        return a->labelset()->less(a->label_of(l),
                                        a->label_of(r));
      };
    for (auto s: a->states())
      if (!detail::is_sorted_forward(a->out(s), less))
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
    }
  }


  /*-------.
  | sort.  |
  `-------*/
  namespace detail
  {
    /// A function to sort an automaton.
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

    public:
      sorter(const input_automaton_t& a)
        : res_(make_shared_ptr<automaton_t>(a))
      {}

      automaton_t operator()()
      {
        visit_and_update_res_();
        push_inaccessible_states_();
        visit_and_update_res_();
        return std::move(res_);
      }

    private:
      void visit_and_update_res_()
      {
        while (! res_->todo_.empty())
          {
            auto p = res_->todo_.front();
            res_->todo_.pop();
            visit_successors_of_(p.first, p.second);
          }
      }

      void visit_successors_of_(input_state_t s, state_t res_s)
      {
        std::vector<input_transition_t> ts;
        // Here a_->out(s) would just as well as a_->all_out(s) but it
        // would be slower; later we have to test one condition per
        // transition anyway, which is just the additional work
        // performed by out.
        for (auto t: res_->input_->all_out(s))
          ts.emplace_back(t);

        // There is a difference in performance between using lambdas
        // or std::bind.  See
        // http://www.gockelhut.com/c++/articles/lambda_vs_bind and
        // especially the bench program
        // http://www.gockelhut.com/c++/files/lambda_vs_bind.cpp.
        //
        // It gives, with -O3
        //
        //                       Clang 3.5    GCC 4.9
        //  lambda                    1001        7000
        //  bind                3716166405  2530142000
        //  bound lambda        2438421993  1700834000
        //  boost bind          2925777511  2529615000
        //  boost bound lambda  2420710412  1683458000
        std::sort(ts.begin(), ts.end(),
                  [&](const input_transition_t t1,
                      const input_transition_t t2) -> bool
                  {
                    return transition_less_(t1, t2);
                  });

        for (auto t: ts)
          res_->new_transition_copy(res_s, res_->state(res_->input_->dst_of(t)),
                                    res_->input_, t);
      }

      void push_inaccessible_states_()
      {
        // States are processed in order.  Like above, a_->states()
        // would work.
        for (auto s: res_->input_->all_states())
          res_->state(s);
      }

      bool transition_less_(const input_transition_t t1,
                                 const input_transition_t t2) const
        ATTRIBUTE_PURE
      {
        // We intentionally ignore source states: they should always
        // be identical when we call this.
        auto& aut = res_->input_;
        assert(aut->src_of(t1) == aut->src_of(t2));
        if (ls_.less(aut->label_of(t1), aut->label_of(t2)))
          return true;
        else if (ls_.less(aut->label_of(t2), aut->label_of(t1)))
          return false;
        else if (ws_.less(aut->weight_of(t1), aut->weight_of(t2)))
          return true;
        else if (ws_.less(aut->weight_of(t2), aut->weight_of(t1)))
          return false;
        else if (aut->dst_of(t1) < aut->dst_of(t2))
          return true;
        else if (aut->dst_of(t2) < aut->dst_of(t1))
          return false;
        else
          return false;
      }

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
    }
  }
}

#endif // !VCSN_ALGOS_SORT_HH
