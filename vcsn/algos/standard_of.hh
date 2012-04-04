#ifndef VCSN_ALGOS_STANDARD_OF_HH
# define VCSN_ALGOS_STANDARD_OF_HH

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>

# define ECHO(S) std::cerr << S << std::endl
# define V(S) #S ": " << S << " "

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
      using alphabet_t = typename automaton_t::alphabet_t;
      using state_t = typename automaton_t::state_t;

      standard_of_visitor(const alphabet_t& alpha,
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
        bool first = true;
        state_t initial = res_.invalid_state();
        weight_t initial_weight = ws_.unit();
        for (auto c: e)
          {
            c->accept(*this);
            if (first)
              {
                initial = initial_;
                initial_weight = initial_weight_;
                first = false;
              }
            else
              {
                // FIXME: initial_weight_!
                for (auto t: res_.out(initial_))
                  res_.add_transition(initial,
                                      res_.dst_of(t),
                                      res_.label_of(t),
                                      res_.weight_of(t));
                res_.del_state(initial_);
              }
          }
        initial_ = initial;
        initial_weight_ = initial_weight;
      }

      virtual void
      visit(const prod<weight_t>&)
      {}

      virtual void
      visit(const star<weight_t>&)
      {}


    private:
      weightset_t ws_;
      automaton_t res_;
      state_t initial_ = automaton_t::invalid_state();
      weight_t initial_weight_ = ws_.unit();
    };

    template <class Aut, class WeightSet>
    Aut
    standard_of(const typename Aut::alphabet_t& alpha, const WeightSet& ws,
                const exp& e)
    {
      standard_of_visitor<Aut, WeightSet> standard(alpha, ws);
      auto res = standard(e);
      return res;
    }

  } // rat::
} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_OF_HH
