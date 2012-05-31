#ifndef VCSN_ALGOS_STANDARD_OF_HH
# define VCSN_ALGOS_STANDARD_OF_HH

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
    template <class Aut,
              class GenSet = typename Aut::genset_t,
              class WeightSet = typename Aut::weightset_t,
              class Kind = typename atom_kind<typename Aut::kind_t>::type>
    class standard_of_visitor
      : public const_visitor<typename atom_trait<Kind, GenSet>::type,
                             typename WeightSet::value_t>
    {
    public:
      using automaton_t = Aut;
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using kind_t = Kind;
      using weight_t = typename weightset_t::value_t;
      using atom_value_t = typename atom_trait<kind_t, genset_t>::type;
      using state_t = typename automaton_t::state_t;

      using super_type = const_visitor<atom_value_t, weight_t>;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      standard_of_visitor(const genset_t& alpha, const weightset_t& ws)
        : ws_(ws)
        , res_(alpha, ws)
      {}

      automaton_t
      operator()(std::shared_ptr<const node_t> v)
      {
        v->accept(*this);
        res_.set_initial(initial_);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        initial_ = res_.new_state();
      }

      virtual void
      visit(const one_t& v)
      {
        auto i = res_.new_state();
        initial_ = i;
        res_.set_final(i, v.left_weight());
      }

      virtual void
      visit(const atom_t& e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        initial_ = i;
        res_.add_transition(i, f, e.value(), e.left_weight());
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
      apply_weights(const inner_t& e, const states_t& other_finals)
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
      visit(const sum_t& e)
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
      visit(const prod_t& e)
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
      visit(const star_t& e)
      {
        states_t other_finals = finals();
        e.sub()->accept(*this);
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

    template <class Aut,
              class GenSet = typename Aut::genset_t,
              class WeightSet = typename Aut::weightset_t,
              class Kind = typename atom_kind<typename Aut::kind_t>::type>
    Aut
    standard_of(const GenSet& alpha, const WeightSet& ws,
                const exp_t e)
    {
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using kind_t = Kind;
      using weight_t = typename weightset_t::value_t;
      using atom_value_t = typename atom_trait<kind_t, genset_t>::type;
      using node_t = rat::node<atom_value_t, weight_t>;
      // Make sure the type is right.
      auto v = std::dynamic_pointer_cast<const node_t>(e);
      assert(v);
      standard_of_visitor<Aut, GenSet, WeightSet, Kind> standard(alpha, ws);
      return standard(v);
    }

  } // rat::
} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_OF_HH
