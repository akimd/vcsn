#ifndef VCSN_ALGOS_STANDARD_OF_HH
# define VCSN_ALGOS_STANDARD_OF_HH

# include <boost/range.hpp>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
    template <class Aut, class WeightSet>
    class standard_of_visitor
      : public visitor_traits<typename WeightSet::value_t>::const_visitor
    {
    public:
      using automaton_t = Aut;
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using super_type = typename visitor_traits<weight_t>::const_visitor;
      using genset_t = typename automaton_t::genset_t;
      using state_t = typename automaton_t::state_t;

      standard_of_visitor(const genset_t& alpha,
                          const weightset_t& ws)
        : ws_(ws)
        , res_(alpha, ws)
      {}

      automaton_t
      operator()(const exp& v)
      {
        down_cast<const node<weight_t>*>(&v)->accept(*this);
        res_.set_initial(initial_, initial_weight_);
        return std::move(res_);
      }

      virtual void
      visit(const zero<weight_t>&)
      {
        initial_ = res_.new_state();
        initial_weight_ = ws_.unit();
      }

      virtual void
      visit(const one<weight_t>& v)
      {
        auto i = res_.new_state();
        initial_ = i;
        initial_weight_ = v.left_weight();
        res_.set_final(i);
      }

      virtual void
      visit(const atom<weight_t>& e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        initial_ = i;
        initial_weight_ = ws_.unit();
        res_.add_transition(i, f, e.get_atom(), e.left_weight());
        res_.set_final(f);
      }

      virtual void
      visit(const sum<weight_t>& e)
      {
        (*e.begin())->accept(*this);
        state_t initial = initial_;
        weight_t initial_weight = initial_weight_;
        for (auto c: boost::make_iterator_range(e, 1, 0))
          {
            c->accept(*this);
            // FIXME: initial_weight_!
            for (auto t: res_.out(initial_))
              res_.add_transition(initial,
                                  res_.dst_of(t),
                                  res_.label_of(t),
                                  res_.weight_of(t));
            res_.del_state(initial_);
          }
        initial_ = initial;
        initial_weight_ = initial_weight;
      }

      virtual void
      visit(const prod<weight_t>& e)
      {
        (*e.begin())->accept(*this);
        state_t initial = initial_;
        weight_t initial_weight = initial_weight_;
        // Store transitions by copy.
        using transs_t = std::vector<typename automaton_t::transition_t>;
        for (auto c: boost::make_iterator_range(e, 1, 0))
          {
            // The set of the current final transitions.
            auto ftrans_ = res_.final_transitions();
            transs_t ftrans{ begin(ftrans_), end(ftrans_) };
            c->accept(*this);
            // Branch all the former final transitions to the new
            // initial state.  FIXME: initial_weight_!
            for (auto t: ftrans)
              {
                res_.set_transition(res_.src_of(t),
                                    initial_,
                                    res_.label_of(t),
                                    res_.weight_of(t));
                res_.del_transition(t);
              }
          }
        initial_ = initial;
        initial_weight_ = initial_weight;
      }

      virtual void
      visit(const star<weight_t>&)
      {}


    private:
      weightset_t ws_;
      automaton_t res_;
      state_t initial_ = automaton_t::null_state();
      weight_t initial_weight_ = ws_.unit();
    };

    template <class Aut, class WeightSet>
    Aut
    standard_of(const typename Aut::genset_t& alpha, const WeightSet& ws,
                const exp& e)
    {
      standard_of_visitor<Aut, WeightSet> standard(alpha, ws);
      auto res = standard(e);
      return res;
    }

  } // rat::
} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_OF_HH
