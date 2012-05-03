#ifndef VCSN_ALGOS_STANDARD_OF_HH
# define VCSN_ALGOS_STANDARD_OF_HH

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
    template <class Aut, class WeightSet>
    class standard_of_visitor
      : public const_visitor<typename WeightSet::value_t>
    {
    public:
      using automaton_t = Aut;
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using super_type = const_visitor<weight_t>;
      using genset_t = typename automaton_t::genset_t;
      using state_t = typename automaton_t::state_t;

      standard_of_visitor(const genset_t& alpha, const weightset_t& ws)
        : ws_(ws)
        , res_(alpha, ws)
      {}

      automaton_t
      operator()(std::shared_ptr<const node<weight_t>> v)
      {
        v->accept(*this);
        res_.set_initial(initial_);
        return std::move(res_);
      }

      virtual void
      visit(const zero<weight_t>&)
      {
        initial_ = res_.new_state();
      }

      virtual void
      visit(const one<weight_t>& v)
      {
        auto i = res_.new_state();
        initial_ = i;
        res_.set_final(i, v.left_weight());
      }

      virtual void
      visit(const atom<weight_t>& e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        initial_ = i;
        res_.add_transition(i, f, e.get_atom(), e.left_weight());
        res_.set_final(f);
      }

      /// The current set of final states.
      using states_t = std::set<state_t>;
      states_t
      finals()
      {
        states_t res;
        for (auto t: res_.final_transitions())
          res.insert(res_.src_of(t));
        return res;
      }

      /// Apply the left weight to initial state, and the right weight
      /// to all the "fresh" final states, i.e., those that are not
      /// part of "other_finals".
      void
      apply_weights(const inner<weight_t>& e,
                    const std::set<state_t>& other_finals)
      {
        {
          weight_t w = e.left_weight();
          if (w != ws_.unit())
            for (auto t: res_.all_out(initial_))
              res_.lmul_weight(t, w);
        }
        {
          weight_t w = e.right_weight();
          if (w != ws_.unit())
            for (auto t: res_.final_transitions())
              if (other_finals.find(res_.src_of(t)) == other_finals.end())
                res_.rmul_weight(t, w);
        }
      }

      virtual void
      visit(const sum<weight_t>& e)
      {
        states_t other_finals = finals();
        e.head()->accept(*this);
        state_t initial = initial_;
        for (auto c: e.tail())
          {
            c->accept(*this);
            for (auto t: res_.all_out(initial_))
              res_.add_transition(initial,
                                  res_.dst_of(t),
                                  res_.label_of(t),
                                  res_.weight_of(t));
            res_.del_state(initial_);
          }
        initial_ = initial;
        apply_weights(e, other_finals);
      }

      virtual void
      visit(const prod<weight_t>& e)
      {
        // The set of the final states that were introduced in pending
        // parts of the automaton (for instance in we are in the rhs
        // of "a+bc", recording the final state for "a").
        states_t other_finals = finals();

        // Traverse the first part of the product, handle left_weight.
        e.head()->accept(*this);
        state_t initial = initial_;

        // Then the remainder.
        for (auto c: e.tail())
          {
            // The set of the current final transitions.
            auto ftr_ = res_.final_transitions();
            // Store transitions by copy.
            using transs_t = std::vector<typename automaton_t::transition_t>;
            transs_t ftr{ begin(ftr_), end(ftr_) };

            // Visit the next member of the product.
            c->accept(*this);

            // Branch all the previously added final transitions to
            // the successors of the new initial state.
            for (auto t1: ftr)
              if (other_finals.find(res_.src_of(t1)) == other_finals.end())
                {
                  for (auto t2: res_.out(initial_))
                    res_.set_transition
                      (res_.src_of(t1),
                       res_.dst_of(t2),
                       res_.label_of(t2),
                       ws_.mul(res_.weight_of(t1), res_.weight_of(t2)));
                  res_.del_transition(t1);
                }
            res_.del_state(initial_);
          }
        initial_ = initial;
        apply_weights(e, other_finals);
      }

      virtual void
      visit(const star<weight_t>& e)
      {
        states_t other_finals = finals();
        e.get_sub()->accept(*this);
        // The "final weight of the initial state", starred.
        weight_t w = ws_.star(res_.get_final_weight(initial_));
        // Branch all the final states (but initial) to the successors
        // of initial.
        for (auto ti: res_.out(initial_))
          {
            res_.lmul_weight(ti, w);
            for (auto tf: res_.final_transitions())
              if (res_.src_of(tf) != initial_
                  && other_finals.find(res_.src_of(tf)) == other_finals.end())
                // Note that the weight of ti has already been
                // multiplied, on the left, by w.
                res_.add_transition
                  (res_.src_of(tf),
                   res_.dst_of(ti),
                   res_.label_of(ti),
                   ws_.mul(res_.weight_of(tf), res_.weight_of(ti)));
          }
        for (auto tf: res_.final_transitions())
          res_.rmul_weight(tf, w);
        res_.set_final(initial_, w);
        apply_weights(e, other_finals);
      }


    private:
      weightset_t ws_;
      automaton_t res_;
      state_t initial_ = automaton_t::null_state();
    };

    template <class Aut, class WeightSet>
    Aut
    standard_of(const typename Aut::genset_t& alpha, const WeightSet& ws,
                const exp_t e)
    {
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      // Make sure the type is right.
      std::shared_ptr<const node<weight_t>> v =
        std::dynamic_pointer_cast<const node<weight_t>>(e);
      assert(v);
      standard_of_visitor<Aut, WeightSet> standard(alpha, ws);
      return standard(v);
    }

  } // rat::
} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_OF_HH
