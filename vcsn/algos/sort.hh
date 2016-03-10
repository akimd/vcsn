#pragma once

#include <vector>

#include <boost/range/algorithm/sort.hpp>

#include <vcsn/core/permutation-automaton.hh>
#include <vcsn/ctx/traits.hh> // base_t
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/algorithm.hh>

namespace vcsn
{

  namespace detail
  {
    /// Compare transitions of an automaton.
    ///
    /// Sort by label, and then by destination state.
    /// To be applied to outgoing transitions of a state.
    template <Automaton Aut>
    struct transition_less
    {
      using automaton_t = Aut;
      using transition_t = transition_t_of<automaton_t>;

      transition_less(const automaton_t& a)
        : aut_(a)
      {}

      ATTRIBUTE_PURE
      bool operator()(const transition_t l,
                      const transition_t r) const
      {
        const auto& llab = aut_->label_of(l);
        const auto& rlab = aut_->label_of(r);
        if (aut_->labelset()->less(llab, rlab))
          return true;
        else if (aut_->labelset()->less(rlab, llab))
          return false;
        else
          return aut_->dst_of(l) < aut_->dst_of(r);
      }

    private:
      automaton_t aut_;
    };
  }

  /*------------------.
  | is_label_sorted.  |
  `------------------*/

  /// Whether for each state, the outgoing transitions are sorted by
  /// increasing label.
  template <Automaton Aut>
  inline
  bool
  is_out_sorted(const Aut& a)
  {
    auto less = detail::transition_less<Aut>{a};
    for (auto s: a->states())
      if (!detail::is_sorted_forward(out(a, s), less))
        return false;
    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
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
    template <Automaton Aut>
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
        // Here out(a_, s) would just as well as all_out(a_, s) but it
        // would be slower; later we have to test one condition per
        // transition anyway, which is just the additional work
        // performed by out.
        for (auto t: all_out(res_->input_, s))
          ts.emplace_back(t);

        boost::sort(ts, detail::transition_less<Aut>{res_->input_});

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

      /// Sorted automaton.
      automaton_t res_;
      const labelset_t_of<input_automaton_t>& ls_ = *res_->input_->labelset();
      const weightset_t_of<input_automaton_t>& ws_ = *res_->input_->weightset();
    }; // class
  } // namespace

  template <Automaton Aut>
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
      template <Automaton Aut>
      automaton
      sort(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::sort(a);
      }
    }
  }
}
