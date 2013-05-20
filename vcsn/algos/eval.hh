#ifndef VCSN_ALGOS_EVAL_HH
# define VCSN_ALGOS_EVAL_HH

# include <vector>

# include <vcsn/core/kind.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/weight.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Aut>
    class evaluator
    {
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");

      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using word_t = typename automaton_t::labelset_t::word_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      // state -> weight.
      using weights = std::vector<weight_t>;

    public:
      evaluator(const automaton_t& a)
        : a_(a)
        , ws_(*a_.weightset())
      {}

      weight_t operator()(const word_t& word) const
      {
        // Initialize
        const weight_t zero = ws_.zero();
        // v1{a_.num_all_states(), zero} is tempting, but the type
        // of zero might result in the compiler believing we are building
        // a vector with two values: a_.num_all_states() and zero.
        weights v1;
        v1.assign(a_.num_all_states(), zero);
        weights v2{v1};

        for (auto init : a_.initial_transitions())
          v1[a_.dst_of(init)] = a_.weight_of(init);

        // Computation
        for (auto l : word)
          {
            v2.assign(v2.size(), zero);
            for (size_t s = 0; s < v1.size(); ++s)
              if (!ws_.is_zero(v1[s])) // delete if bench >
                for (auto tr : a_.out(s, l))
                  // Introducing a reference to v2[a_.dst_of(tr)] is
                  // tempting, but won't work for std::vector<bool>.
                  // FIXME: Specialize for Boolean?
                  v2[a_.dst_of(tr)] =
                    ws_.add(v2[a_.dst_of(tr)],
                            ws_.mul(v1[s], a_.weight_of(tr)));
              // (1) w = zero.
            std::swap(v1, v2);
          }
        weight_t res = zero;
        for (auto f : a_.final_transitions())
          res = ws_.add(res,
                        ws_.mul(v1[a_.src_of(f)], a_.weight_of(f)));
        return res;
      }
    private:
      const automaton_t& a_;
      const weightset_t& ws_;
    };

  } // namespace detail

  template <class Aut>
  inline
  auto
  eval(const Aut& a, const typename Aut::labelset_t::word_t& w)
    -> typename Aut::weight_t
  {
    detail::evaluator<Aut> e(a);
    return e(w);
  }

  /*----------------.
  | abstract eval.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      auto
      eval(const automaton& aut, const std::string& s)
        -> dyn::weight
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        const auto& ctx = a.context();
        auto res = ::vcsn::eval(a, s);
        return make_weight(ctx, res);
      }

      REGISTER_DECLARE(eval,
                       (const automaton& aut, const std::string& s)
                       -> dyn::weight);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_EVAL_HH
