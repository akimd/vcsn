#ifndef VCSN_ALGOS_STANDARD_OF_HH
# define VCSN_ALGOS_STANDARD_OF_HH

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
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using super_type = typename visitor_traits<weight_t>::const_visitor;
      using alphabet_t = typename Aut::alphabet_t;

      standard_of_visitor(const typename Aut::alphabet_t& alpha,
                          const WeightSet& ws)
        : res_(alpha, ws)
      {}

      Aut
      operator()(const exp& v)
      {
        down_cast<const node<weight_t>*>(&v)->accept(*this);
        return std::move(res_);
      }

      virtual void
      visit(const prod<weight_t>&)
      {}

      virtual void
      visit(const sum<weight_t>&)
      {}

      virtual void
      visit(const star<weight_t>&)
      {}

      virtual void
      visit(const one<weight_t>&)
      {
        auto i = res_.new_state();
        res_.set_initial(i);
        res_.set_final(i);
      }

      virtual void
      visit(const zero<weight_t>&)
      {
        auto i = res_.new_state();
        res_.set_initial(i);
      }

      virtual void
      visit(const atom<weight_t>& e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        res_.set_initial(i);
        res_.add_transition(i, f, e.get_atom(), e.left_weight());
        res_.set_final(f);
      }

    private:
      Aut res_;
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
