#ifndef VCSN_ALGOS_ARE_ISOMORPHIC_HH
# define VCSN_ALGOS_ARE_ISOMORPHIC_HH

# include <map>
# include <stack>
# include <unordered_map>
# include <unordered_set>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/hash.hh>

namespace vcsn
{

  /*-----------------.
  | are_isomorphic.  |
  `-----------------*/

  template <typename Aut1, typename Aut2>
  class are_isomorphicer // FIXME: this naming convention is really getting too silly
  {
  private:
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
                                          vcsn::hash<labelset_t>,
                                          vcsn::equal_to<labelset_t>>>
      out1_, out2_;

    /// The maps associating the states of a1_ and the states of a2_.
    std::unordered_map<state1_t, state2_t> s1tos2_;
    std::unordered_map<state2_t, state1_t> s2tos1_;

    /// A worklist of state pairs which are candidate to be
    /// isomorphic.  Or "A candidate-isomorphic state pair worklist",
    /// written in Reverse-Polish English.
    std::stack<std::pair<state1_t, state2_t>> worklist_;

    /// A datum specifying if two given automata are isomorphic, and
    /// why if they are not.  This should be a variant record, but
    /// BOOST variants are not really suitable (this is not just a
    /// disjoint union: we also need a case tag), we don't like
    /// unions, and visitors are overkill.  The thing might even get
    /// simpler when we generalize to non-deterministic automata.
    struct full_response
    {
      enum class tag
      {
        never_computed = -1,      // We didn't run the algorithm yet.
        isomorphic = 0,           // a1_ and a2_ are in fact isomorphic.
        counterexample = 1,       // We found a counterexample of two states
                                  // [which is only possible for DFAs].
        trivially_different  = 2, // Different number of states or transitions.
      } response;

      /// Only meaningful if the tag is tag::counterexample.
      std::pair<state_t, state_t> counterexample;

      full_response()
        : response(tag::never_computed)
      {}
    } full_response_;

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
      // Automata of different sizes are different.
      if (a1_.num_states() != a2_.num_states())
        return true;
      if (a1_.num_transitions() != a2_.num_transitions())
        return true;

      // The idea of comparing alphabet sizes here is tempting, but
      // it's more useful to deal with actually used labels only; we
      // consider isomorphic even two automata from labelsets with
      // different alphabets, when used labels match.  Building a set
      // of actually used labels has linear complexity, and it's not
      // obvious that performing an additional check now would pay in
      // real usage.  FIXME: benchmark in real cases.

      return false;
    }

  public:
    are_isomorphicer(const Aut1 &a1, const Aut2 &a2)
      : a1_(a1)
      , a2_(a2)
    {}

    const full_response
    get_full_response()
    {
      require(is_accessible(a1_) && is_accessible(a2_),
              "are-isomorphic: input automata must both be accessible");

      // FIXME: this only works on lal, and the algorithm is
      // instantiated only on lal contexts anyway.  It would be nice
      // to generalize this to some non-lal contexts later, but the
      // current implementation only is limited to deterministic lal
      // automata anyway.
      require(is_deterministic(a1_) && is_deterministic(a2_),
              "are-isomorphic: input automata must both be deterministic"
              " (at this time)");

      // If we prove non-isomorphism at this point, it will be because
      // of sizes.
      full_response_.response = full_response::tag::trivially_different;

      // Before even initializing our data structures, which is
      // potentially expensive, try to simply compare the number of
      // elements such as states and transitions.
      if (trivially_different())
        return full_response_;

      clear();
      fill_outs_();

      // If we prove non-isomorphism from now on, it will be by
      // presenting some specific pair of states.
      full_response_.response = full_response::tag::counterexample;

      worklist_.push({a1_.pre(), a2_.pre()});

      while (! worklist_.empty())
        {
          const states_t states = worklist_.top(); worklist_.pop();
          const state_t s1 = states.first, s2 = states.second;

          // If we prove non-isomorphism in this iteration, it will be
          // by using the <s1, s2> pair as a counterexample.
          full_response_.counterexample = {s1, s2};

          // If the number of transitions going out from the two
          // states is different, don't even bother looking at them.
          // On the other hand if the number is equal we can afford to
          // reason in just one direction: look at transitions from s1
          // and ensure that all of them have a matching one from s2.
          if (out1_[s1].size() != out2_[s2].size())
            return full_response_;

          for (const auto& l1_w1dst1 : out1_[s1]) // out1_.at(s1) may fail.
            {
              const label_t& l1 = l1_w1dst1.first;
              const weight_t& w1 = l1_w1dst1.second.first;
              const state_t& dst1 = l1_w1dst1.second.second;

              const auto& s2out = out2_.find(s2);
              if (s2out == out2_.cend())
                return full_response_;
              const auto& s2outl = s2out->second.find(l1);
              if (s2outl == s2out->second.cend())
                return full_response_;
              weight_t w2 = s2outl->second.first;
              state_t dst2 = s2outl->second.second;

              if (! weightset_t::equals(w1, w2))
                return full_response_;

              const auto& isomorphics_to_dst1 = s1tos2_.find(dst1);
              const auto& isomorphics_to_dst2 = s2tos1_.find(dst2);

              if (isomorphics_to_dst1 == s1tos2_.cend())
                {
                  if (isomorphics_to_dst2 == s2tos1_.cend()) // Both are empty.
                    {
                      s1tos2_[dst1] = dst2;
                      s2tos1_[dst2] = dst1;
                      worklist_.push({dst1, dst2});
                    }
                  else
                    return full_response_;
                }
              else if (isomorphics_to_dst1 == s1tos2_.cend()
                       || isomorphics_to_dst1->second != dst2
                       || isomorphics_to_dst2->second != dst1)
                  return full_response_;
            } // outer for
        } // while
      full_response_.response = full_response::tag::isomorphic;
      return full_response_;
    }

    bool operator()()
    {
      const full_response& r = get_full_response();
      return full_response_.response == full_response::tag::isomorphic;
    }

    /// A map from each a2_ state to the corresponding a1_ state.
    using origins_t = std::map<state_t, state_t>;

    /// Only meaningful if operator() returned true.
    origins_t
    origins()
    {
      require(full_response_.reponse == full_response::tag::isomorphic,
              __func__, ": isomorphism-check not successfully performed");
      origins_t res;
      for (const auto& s2s1: s2tos1_)
        res[s2s1.first] = s2s1.second;
      return res;
    }

    /// Print origins.
    static
    std::ostream&
    print(std::ostream& o, const origins_t& orig)
    {
      o << "/* Origins." << std::endl
        << "    node [shape = box, style = rounded]" << std::endl;
      for (auto p : orig)
        if (2 <= p.first)
          {
            o << "    " << p.first - 2
              << " [label = \"" << p.second << "\"]" << std::endl;
          }
      o << "*/" << std::endl;
      return o;
    }

  };

  template <typename Aut1, typename Aut2>
  bool
  are_isomorphic(const Aut1& a1, const Aut2& a2)
  {
    are_isomorphicer<Aut1, Aut2> are_isomorphic(a1, a2);

    return are_isomorphic();
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
