#ifndef VCSN_ALGOS_ARE_ISOMORPHIC_HH
# define VCSN_ALGOS_ARE_ISOMORPHIC_HH

# include <vcsn/dyn/automaton.hh> // FIXME: remove unless needed
# include <vcsn/dyn/fwd.hh>
# include <unordered_set> // FIXME: only keep if needed
# include <unordered_map> // FIXME: only keep if needed
# include <set> // FIXME: only keep if needed
# include <map> // FIXME: only keep if needed
# include <stack> // FIXME: only keep if needed
# include <queue> // FIXME: only keep if needed
# include <vcsn/algos/is-deterministic.hh> // FIXME: only keep if needed
# include <vcsn/algos/accessible.hh>
# include <vcsn/labelset/hasher-labelset.hh>
# include <vcsn/labelset/equalto-labelset.hh>


namespace vcsn
{

  /*-----------------.
  | are_isomorphic.  |
  `-----------------*/

  template <typename Aut1, typename Aut2>
  class are_isomorphicer // FIXME: this naming convention is really getting too silly
  {
    using automaton1_t = Aut1;
    using context1_t = typename automaton1_t::context_t;
    using weightset1_t = typename automaton1_t::weightset_t;
    using labelset1_t = typename context1_t::labelset_t;
    using state1_t = typename automaton1_t::state_t;
    using label1_t = typename automaton1_t::label_t;
    using weight1_t = typename automaton1_t::weight_t;
    using transition1_t = typename automaton1_t::transition_t;

    using automaton2_t = Aut2;
    using context2_t = typename automaton2_t::context_t;
    using weightset2_t = typename automaton1_t::weightset_t;
    using labelset2_t = typename context2_t::labelset_t;
    using state2_t = typename automaton2_t::state_t;
    using label2_t = typename automaton2_t::label_t;
    using weight2_t = typename automaton2_t::weight_t;
    using transition2_t = typename automaton2_t::transition_t;

    /// This assumes that typaname Aut1 and typename Aut2 are the same.
    using automaton_t = automaton1_t;
    using weightset_t = weightset1_t;
    using context_t = context1_t;
    using labelset_t = labelset1_t;
    using state_t = state1_t;
    using label_t = label1_t;
    using weight_t = weight1_t;
    using transition_t = transition1_t;

    using states_t = std::pair<state1_t, state2_t>;

    const Aut1& a1_;
    const Aut2& a2_;

    /// See the comment for out_ in minimize.hh.
    std::unordered_map<state_t,
                       std::unordered_map<label_t,
                                          std::pair<weight_t, state_t>,
                                          ctx::label_hasher<labelset_t>,
                                          ctx::label_equal_to<labelset_t>>>
      out1_, out2_;

    /// The maps associating the states of a1_ and the states of a2_.
    std::unordered_map<state1_t, state2_t> s1tos2;
    std::unordered_map<state2_t, state1_t> s2tos1;

    /// A worklist of state pairs which are candidate to be isomorphic.
    /// Or "A candidate-isomorphic state pair worklist", written in Reverse-Polish English.
    std::stack<std::pair<state_t, state_t>> worklist;

    /// To be set in case of non-isomorphism, as a counterexample.
    // FIXME: use this: Akim requested it, but I haven't done it yet.
    // This can only work in the deterministic case.
    // FIXME: add some "origins" functionality, to be used in case of
    // isomorphism.
    std::pair<state_t, state_t> counterexample;

    void fill_outs_()
    {
      for (auto t1 : a1_.all_transitions())
        out1_[a1_.src_of(t1)][a1_.label_of(t1)] = {a1_.weight_of(t1),
                                                   a1_.dst_of(t1)};
      for (auto t2 : a2_.all_transitions())
        out2_[a2_.src_of(t2)][a2_.label_of(t2)] = {a2_.weight_of(t2),
                                                   a2_.dst_of(t2)};
    }

    void clear()
    {
      out1_.clear();
      out2_.clear();
    }

    bool trivially_different()
    {
      /* Automata of different sizes are different. */
      if (a1_.num_states() != a2_.num_states())
        return true;
      if (a1_.num_transitions() != a2_.num_transitions())
        return true;

      /* If labels are different then the automata are different. */
      std::unordered_set<label1_t> labels1;
      std::unordered_set<label2_t> labels2;
      for (auto l : labels1)
        labels1.emplace(l);
      for (auto l : labels2)
        labels2.emplace(l);
      if (labels1.size() != labels2.size())
        return true;
      // FIXME: Isn't there some reasonable way of getting the labelset size?
      for (auto l : labels1)
        if (labels2.find(l) == labels2.end())
          return true;
      // FIXME: all this work on labels is questionable.  I should
      // check the actually used labels instead of labelsets.

      return false;
    }

  public:
    are_isomorphicer(const Aut1 &a1, const Aut2 &a2)
      : a1_(a1)
      , a2_(a2)
    {}

    bool operator()()
    {
      //      if (! is_deterministic(a1_) || ! is_deterministic(a2_))
      //        abort();
      if (! is_accessible(a1_) || ! is_accessible(a2_))
        abort();

      // Before even initializing our data structures, which is
      // potentially expensive, try to simply compare the number of
      // states, transitions and labels.
      if (trivially_different())
        return false;

      clear();
      fill_outs_();

      worklist.push({a1_.pre(), a2_.pre()});

      while (! worklist.empty())
        {
          const states_t states = worklist.top(); worklist.pop();
          const state_t s1 = states.first, s2 = states.second;
          //std::cerr << "Popped <" << states.first << ", " << states.second << ">\n";

          for (const auto& l1_w1dst1 : out1_[s1]) // out1_.at(s1) may fail.
            {
              const label_t& l1 = l1_w1dst1.first;
              const weight_t& w1 = l1_w1dst1.second.first;
              const state_t& dst1 = l1_w1dst1.second.second;

              //std::cerr << "Considering " << dst1 << "...\n";
              // out2_.at(s2) fails is s2 has no output transitions.
              const auto& s2out = out2_.find(s2);
              if (s2out == out2_.cend())
                return false;
              const auto& s2outl = s2out->second.find(l1);
              if (s2outl == s2out->second.cend())
                return false;
              weight_t w2 = s2outl->second.first;
              state_t dst2 = s2outl->second.second;

              if (! weightset_t::equals(w1, w2))
                return false;

              // std::cerr << "Considering " << dst1 << " and " << dst2 << "... ";
              // std::cerr << "["
              //           << (s1tos2.find(dst1) != s1tos2.cend())
              //           << ", "
              //           << (s2tos1.find(dst2) != s2tos1.cend())
              //           << "]\n";

              // FIXME: move to fields.
              const auto& isomorphics_to_dst1 = s1tos2.find(dst1);
              const auto& isomorphics_to_dst2 = s2tos1.find(dst2);

              ////////
              // if (isomorphics_to_dst1 != s1tos2.cend()
              //     && isomorphics_to_dst2 != s2tos1.cend())
              //   std::cerr << "["
              //             << s1tos2.at(dst1)
              //             << ", "
              //             << s2tos1.at(dst2)
              //             << "]\n";
              ////////
              if (isomorphics_to_dst1 == s1tos2.cend())
                {
                  //std::cerr << "OK 700\n";
                  if (isomorphics_to_dst2 == s2tos1.cend()) // Both are empty.
                    {
                      s1tos2[dst1] = dst2;
                      s2tos1[dst2] = dst1;
                      worklist.push({dst1, dst2});
                      //std::cerr << "Pushed <" << dst1 << ", " << dst2 << ">\n";
                    }
                  else
                    return false;
                }
              else
                {
                  //std::cerr << "OK 800\n";
                  if (isomorphics_to_dst1 == s1tos2.cend())
                    {
                      //std::cerr << "OK 850\n";
                      return false;
                    }
                  else // Neither is empty.
                    if ((isomorphics_to_dst1->second != dst2)
                        || (isomorphics_to_dst2->second != dst1))
                      {
                        //std::cerr << "OK 900\n";
                        return false;
                      }
                }
              //std::cerr << "OK 1000\n";

              // if ((isomorphics_to_dst1 == s1tos2.cend())
              //     != (isomorphics_to_dst2 == s2tos1.cend()))
              //   return false;
            } // outer for
        } // while
      // std::set<const label_t> alphabet1 = *a1_.labelset();
      // std::set<const label_t> alphabet2 = *a2_.labelset();
      return true; // FIXME: do it for real
    }
    // using weightset1_t
    // = decltype(join(std::declval<typename Lhs::weightset_t>(),
    //                     std::declval<typename Rhs::weightset_t>()));
  };

  template <typename Aut1, typename Aut2>
  bool
  are_isomorphic(const Aut1& a1, const Aut2& a2)
  {
    are_isomorphicer<Aut1, Aut2> are_isomorphic(a1, a2);

    return are_isomorphic();
    // // FIXME: do it for real.
    // (void) a1;
    // (void) a2;
    // return false;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Aut1, typename Aut2>
      bool
      are_isomorphic(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = aut1->as<Aut1>();
        const auto& a2 = aut2->as<Aut2>();
        return are_isomorphic(a1, a2);
      }

      REGISTER_DECLARE(are_isomorphic,
                       (const automaton&, const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_ARE_ISOMORPHIC_HH
