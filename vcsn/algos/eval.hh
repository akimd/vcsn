#ifndef VCSN_ALGOS_EVAL_HH
# define VCSN_ALGOS_EVAL_HH

# include <vector>
# include <utility>
# include <cassert>

//# include <vcsn/core/crange.hh>

namespace vcsn {

  template <class Aut>
  class eval_functor
  {
    using automaton_t = Aut;
    using word_t = typename automaton_t::genset_t::word_t;
    using weightset_t = typename automaton_t::weightset_t;
    using weight_t = typename weightset_t::value_t;

  public:
    eval_functor(const automaton_t& a)
    : a_(a)
    , w_(a_.weightset())
    {}

    weight_t operator()(const word_t& w) const
    {
      // if the automaton isn't empty
      // KLUDGE: We want to use `automaton_t::empty()'
      if (a_.num_states() == 0)
        return w_.zero();

      // Initialize
      const size_t size = w.size() + 1;
      weight_t res = w_.zero();
      auto initial_transitions = a_.initial_transitions();
      auto it = initial_transitions.begin();
      auto end_it = initial_transitions.end();

      if(it == end_it)
        return res; // No initial states

      std::vector<std::pair<decltype(end_it), decltype(end_it)>> tr_stack;
      tr_stack.reserve(size);
      tr_stack.push_back(std::make_pair(a_.out(*it).begin(), a_.out(*it).end()));

      // Create lambda
      auto next_path = [&] {
        while (!tr_stack.empty() && (++(tr_stack.back().first) == tr_stack.back().second))
          {
            tr_stack.pop_back();
          }
      };
      auto reduce = [&] () -> weight_t {
        weight_t accu = w_.unit();
        // FIXME: Check if we apply `mul' function in the good direction
        for (auto s : tr_stack)
          {
            // KLUDGE: operator *= or equivalent (temp var)
            accu = w_.mul(accu, a_.weight_of(*s.first));
          }
        return accu;
      };

      while (!tr_stack.empty())
        {
          auto out_container = a_.out(a_.dst_of(*(tr_stack.back().first)), w[tr_stack.size() - 1]);

          auto it = out_container.begin();
          auto end = out_container.end();
          if (end == it)
            // No path to continue
            // Reduce the stack to find next path
            next_path();
          else
            {
              tr_stack.push_back(std::make_pair(it, end));
              if (tr_stack.size() == size)
                {
                  // Each transitions read the whole word
                  while (tr_stack.back().first != tr_stack.back().second)
                    {
                      // Get the weight for each path
                      if (a_.is_final(a_.dst_of((*tr_stack.back().first))))
                        res = w_.add(res, reduce()); // FIXME: The weight could be accumulate before
                      ++(tr_stack.back().first);
                    }
                  tr_stack.pop_back();
                  next_path();
                }
            }
        }

      return res;
    }
  private:
    const automaton_t& a_;
    const weightset_t& w_;
  };

  template <class Aut>
  inline
  typename Aut::weight_t
  eval(const Aut& a, const typename Aut::genset_t::word_t& w)
  {
    eval_functor<Aut> evalf(a);
    return evalf(w);
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_EVAL_HH
