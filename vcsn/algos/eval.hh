#ifndef VCSN_ALGOS_EVAL_HH
# define VCSN_ALGOS_EVAL_HH

# include <cassert>
# include <map>
# include <utility>
# include <vector>

# include <boost/lexical_cast.hpp>

# include <vcsn/core/kind.hh>

namespace vcsn
{
  namespace details
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
      using weights = std::map<state_t, weight_t>;

    public:
      evaluator(const automaton_t& a)
        : a_(a)
        , ws_(*a_.weightset())
      {}

      weight_t operator()(const word_t& word) const
      {
        // Initialize
        const weight_t zero = ws_.zero();
        weights v1;
        weights v2;

        for (auto init : a_.initial_transitions())
          v1[a_.dst_of(init)] = a_.weight_of(init);

        // Computation
        for (auto l : word)
          {
            for (auto& w : v2) // FIXME: (1) set zero
              w.second = zero;
            for (auto w : v1)
              if (zero != w.second) // delete if bench >
                for (auto tr : a_.out(w.first, l))
                  {
                    auto& weight = v2[a_.dst_of(tr)];
                    weight =
                      ws_.add(weight,
                              ws_.mul(w.second, a_.weight_of(tr)));
                  }
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

  } // namespace details

  template <class Aut>
  inline
  auto
  eval(const Aut& a, const typename Aut::labelset_t::word_t& w)
    -> typename Aut::weight_t
  {
    details::evaluator<Aut> e(a);
    return e(w);
  }

  /*----------------.
  | abstract eval.  |
  `----------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      auto
      eval(const automaton& aut, const std::string& s)
        -> std::string
      {
        auto res = ::vcsn::eval(dynamic_cast<const Aut&>(*aut), s);
        return boost::lexical_cast<std::string>(res);
      }

      using eval_t = auto (const automaton& aut, const std::string& s)
        -> std::string;

      bool eval_register(const std::string& ctx, eval_t fn);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_EVAL_HH
