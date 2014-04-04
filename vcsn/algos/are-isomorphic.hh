/// FIXME: add a comment explaining why we say "sequential" instead of
/// "deterministic".

#ifndef VCSN_ALGOS_ARE_ISOMORPHIC_HH
# define VCSN_ALGOS_ARE_ISOMORPHIC_HH

# include <map>
# include <stack>
# include <map> // FIXME: remove unless needed
# include <unordered_map>
# include <unordered_set>
# include <vector> // FIXME: remove unless needed

# include <vcsn/algos/accessible.hh>
//# include <vcsn/algos/is-sequential.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/backtracker.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/map.hh> // vcsn::less FIXME: remove unless needed

// namespace nqueens
// {
// std::ostream& operator<<(std::ostream& o, const std::vector<int>& v)
// {
//   o << "[ ";
//   for (const auto i: v)
//     o << i << " ";
//   o << "]";
//   return o;
// }

// using queen_t = int;

// struct queens_t: public std::vector<queen_t> // queen rows; columns are implicit
// {
//   using move_t = queen_t;

//   size_t queen_no;
// };

// struct mybacktracker: public vcsn::backtracker<queens_t, queen_t, mybacktracker>
// {
//   void do_move(queens_t& s, const queen_t& c)
//   {
//     s.emplace_back(c);
//   }
//   void undo_move(queens_t& s, const queen_t& c __attribute__((unused)))
//   {
//     s.pop_back();
//   }

//   void set_initial(const solution_t& solution)
//   {
//     solution_ = solution;
//   }

//   bool doomed(const move_t* latest_move)
//   {
//     if (latest_move == NULL)
//       return false;

//     int current_size = solution_.size();
//     int j = current_size - 1;
// #define COLUMN(i) (i)
// #define ROW(i) (solution_[i])
// #define DIAGONAL1(i) (COLUMN(i) - ROW(i))
// #define DIAGONAL2(i) (COLUMN(i) + ROW(i))
//     for (int i = 0; i < current_size - 1; ++ i)
//       if (COLUMN(i) == COLUMN(j)
//           || ROW(i) == ROW(j)
//           || DIAGONAL1(i) == DIAGONAL1(j)
//           || DIAGONAL2(i) == DIAGONAL2(j))
//         return true; // One old queen is menacing our new one.
//     return false;
//   }

//   bool final(const move_t* latest_move)
//   {
//     bool res = ! doomed(latest_move) && solution_.size() == solution_.queen_no;
//     //std::cerr << "Is " << solution_ << " final? (size is " << solution_.size() << ", over " << solution_.queen_no << "): " << res << "\n";
//     return res;
//   }

//   void develop(const move_t* latest_move)
//   {
//     //std::cerr << "Depth " << depth() << "\n";
//     //std::cerr << depth() << " ";
//     ++ vcsn::candidate_solution_no;
//     //std::cerr << "Considering " << solution_ << "\n";
//     //int solution_size = solution_.size();

//     if (! doomed(latest_move))
//       for (move_t row = 0; row < move_t(solution_.queen_no); ++ row)
//         push_move(row);
//   }
// };
// } // namespace nqueens


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
    const size_t a1_state_no_;

    /// See the comment for out_ in minimize.hh.
    using dout_t =
      std::unordered_map<state_t,
                         std::unordered_map<label_t,
                                            std::pair<weight_t, state_t>,
                                            vcsn::hash<labelset_t>,
                                            vcsn::equal_to<labelset_t>>>;
    dout_t dout1_, dout2_; // For the simpler, faster sequential case.
    // FIXME: factor the type, just for indentation's sake
    std::unordered_map<state_t,
                       std::unordered_map<label_t,
                                          std::unordered_map<weight_t,
                                                             std::vector<state_t>,
                                                             vcsn::hash<weightset_t>,
                                                             vcsn::equal_to<weightset_t>>,
                                          vcsn::hash<labelset_t>,
                                          vcsn::equal_to<labelset_t>>>
      nout1_, nout2_; // For the nonsequential case.

    /// The maps associating the states of a1_ and the states of a2_.
    using s1tos2_t = std::unordered_map<state1_t, state2_t>;
    using s2tos1_t = std::unordered_map<state2_t, state1_t>;
    s1tos2_t s1tos2_;
    s2tos1_t s2tos1_;
    // FIXME: remove these, and use an isomorphism_t-typed member instaed.

    struct isomorphism_t
    {
      s1tos2_t s1tos2;
      s2tos1_t s2tos1;
      bool doomed;
    };

    /// A worklist of state pairs which are candidate to be
    /// isomorphic.  Or "A candidate-isomorphic state pair worklist",
    /// written in Reverse-Polish English.
    using pair_t = std::pair<state1_t, state2_t>;
    using worklist_t = std::stack<pair_t>;
    worklist_t worklist_;
    class dummy{}; using dummy_t = dummy;

    //????
    struct move
    {
      state1_t s1;
      state2_t s2;
    };
    using move_t = move;
    class mybacktracker: public vcsn::backtracker<isomorphism_t, move_t, dummy_t,
                                                  mybacktracker>
    {
    public:
      are_isomorphicer& outer_;

      // FIXME: is this a good idea?
      const are_isomorphicer::automaton1_t& a1_;
      const are_isomorphicer::automaton2_t& a2_;

      mybacktracker(are_isomorphicer& containing_class)
        : outer_(containing_class)
        , a1_(outer_.a1_)
        , a2_(outer_.a2_)
      {
        //std::cerr << "OK-D\n";
      }
      using super_t = vcsn::backtracker<isomorphism_t, move_t, dummy_t,
                                        mybacktracker>;
      using solution_t = typename super_t::solution_t;
      using move_t = typename super_t::move_t;
      using undo_info_t = typename super_t::undo_info_t;

      void do_move(solution_t& s, const move_t& m)
      {
        //std::cerr << "OK-N 100\n";
        std::cerr << "+ DOING   " << m.s1 << ", " << m.s2 << "\n";
        make_isomorphic(s, m.s1, m.s2);
        //std::cerr << "OK-N 200\n";
      }
      void undo_move(solution_t& s, const move_t& m, const undo_info_t&)
      {
        std::cerr << "- UNdoing " << m.s1 << ", " << m.s2 << "\n";
        state1_t s1 = m.s1;
        state2_t s2 = m.s2;
        assert(are_isomorphic(s, s1, s2));
        assert(! are_nonisomorphic(s, s1, s2));
        s.s1tos2.erase(s1);
        s.s1tos2.erase(s2);
        s.doomed = false;
        assert(! are_isomorphic(s, s1, s2));
        assert(! are_nonisomorphic(s, s1, s2));
      }
      const undo_info_t undo_info(const solution_t&, const move_t&)
      {
        //std::cerr << "OK L\n";
        return undo_info_t();
      }
      void set_initial(const solution_t& initial)
      {
        super_t::solution_ = initial;
        // maybe
        super_t::push_move({a1_.pre(), a2_.pre()});
      }
      bool final(const move_t*)
      {
        bool res =
          ! super_t::solution_.doomed
          && super_t::solution_.s1tos2.size() == outer_.a1_state_no_;
        std::cerr << "Final (size " << super_t::solution_.s1tos2.size() << " of " << outer_.a1_state_no_<< ")? " << res << "\n";
        return res;
      }
      bool are_isomorphic(solution_t& s, state1_t s1, state2_t s2)
      {
        const auto& s1tos2 = s.s1tos2;
        const auto& s2tos1 = s.s2tos1;
        // std::cerr << "OK-O s1tos2.size(): " << s1tos2.size() << "\n";
        // std::cerr << "OK-O s2tos1.size(): " << s2tos1.size() << "\n";
        // std::cerr << "OK-O s1: " << s1 << "\n";
        // std::cerr << "OK-O s2: " << s2 << "\n";
        const auto& s1p = s1tos2.find(s1);
        const auto& s2p = s2tos1.find(s2);
        // std::cerr << "OK-O s1p != s1tos2.end(): " << (s1p != s1tos2.end()) << "\n";
        // if (s1p != s1tos2.end())
        //   std::cerr << "OK-O s1p->second == s2: " << (s1p->second == s2) << "\n";
        // std::cerr << "OK-O s2p != s2tos1.end(): " << (s2p != s2tos1.end()) << "\n";
        // if (s2p != s2tos1.end())
        //   std::cerr << "OK-O s2p->second == s1: " << (s2p->second == s1) << "\n";
        bool res = s1p != s1tos2.end() && s1p->second == s2
          && s2p != s2tos1.end() && s2p->second == s1;
        // std::cerr << "OK-O res: " << res << "\n";
        return res;
      }
      /// Asserting the non-isomorphism of two states entails the
      /// production of a *proof* of the fact that they are not
      /// isomorphic: this in practice means that at least one of them
      /// is known to be isomorphic to a different state.
      /// Non-isomorphism is different from the lack of a known
      /// isomorphism between two states.
      bool are_nonisomorphic(solution_t& s, state1_t s1, state2_t s2)
      {
        const auto& s1tos2 = s.s1tos2;
        const auto& s2tos1 = s.s2tos1;
        const auto& s1p = s1tos2.find(s1);
        const auto& s2p = s2tos1.find(s2);
        return (s1p != s1tos2.end() && s1p->second != s2)
               || (s2p != s2tos1.end() && s2p->second != s1);
      }
      void make_isomorphic(solution_t& s, state1_t s1, state2_t s2)
      {
        // std::cerr << "OK-M 100\n";
        assert(! are_isomorphic(s, s1, s2));
        assert(! are_nonisomorphic(s, s1, s2));
        auto& s1tos2 = s.s1tos2;
        auto& s2tos1 = s.s2tos1;
        assert(s1tos2.find(s1) == s1tos2.end());
        assert(s2tos1.find(s2) == s2tos1.end());
        std::cerr << "Making " << s1 << " and " << s2 << " isomorphic.\n";
        s1tos2[s1] = s2;
        s2tos1[s2] = s1;
        assert(s1tos2.find(s1) != s1tos2.end());
        assert(s2tos1.find(s2) != s2tos1.end());
        // std::cerr << "Now " << s1 << " and " << s2 << " should be isomorphic.\n";
        assert(are_isomorphic(s, s1, s2));
        // std::cerr << "OK-M 100\n";
      }
      void develop(const move_t* latest_move)
      {
        // std::cerr << "OK-E\n";
        if (latest_move == NULL
            || super_t::solution_.doomed)
          return;

        state1_t s1 = latest_move->s1;
        state1_t s2 = latest_move->s2;
        assert(are_isomorphic(super_t::solution_, s1, s2));

        // FIXME: develop successors.
      }
    } backtracker_;
    //????

    /// A datum specifying if two given automata are isomorphic, and
    /// why if they are not.  This should be a variant record, but
    /// BOOST variants are not really suitable (this is not just a
    /// disjoint union: we also need a case tag), we don't like
    /// unions, and visitors are overkill.  The thing might even get
    /// simpler when we generalize to non-sequential automata.
    struct full_response
    {
      enum class tag
      {
        never_computed = -1,      // We didn't run the algorithm yet.
        isomorphic = 0,           // a1_ and a2_ are in fact isomorphic.
        counterexample = 1,       // We found a counterexample of two states
                                  // [which is only possible for DFAs].
        nocounterexample = 2,     // Exhaustive tests failed [we only do this
                                  // expensive check for NFAs].
        trivially_different  = 3, // Different number of states or transitions.
      } response;

      /// Only meaningful if the tag is tag::counterexample.
      std::pair<state1_t, state2_t> counterexample;

      full_response()
        : response(tag::never_computed)
      {}
    } full_response_;

    // Return true and fill \a dout if \a a is sequential;
    // otherwise return false and clear dout.
    bool is_sequential_filling(const automaton_t& a, dout_t& dout)
    {
      for (auto t : a.all_transitions())
        {
          state_t src = a.src_of(t);
          label_t l = a.label_of(t);
          auto& doutsrc = dout[src];
          if (doutsrc.find(l) == doutsrc.end())
            dout[src][l] = {a.weight_of(t), a.dst_of(t)};
          else
            {
              dout.clear();
              std::cerr << "The automaton is NOT sequential.\n";
              return false;
            }
        }
      std::cerr << "The automaton IS sequential.\n";
      return true;
    }

    // Return true iff both automata are sequential, and we can
    // apply the faster version.  We can't use the is_sequential
    // algorithm as it's only defined for lal.  We seize the occasion
    // for filling dout1_ and dout2_ in the sequential case.
    bool are_automata_sequential_filling_douts()
    {
      bool res = is_sequential_filling(a1_, dout1_)
                 && is_sequential_filling(a2_, dout2_);
      std::cerr << "Are both automata sequential? " << res << "\n";
      return res;
    }

    void fill_nouts_()
    {
      for (auto t1 : a1_.all_transitions())
        nout1_[a1_.src_of(t1)][a1_.label_of(t1)][a1_.weight_of(t1)]
          .emplace_back(a1_.dst_of(t1));
      for (auto t2 : a2_.all_transitions())
        nout2_[a2_.src_of(t2)][a2_.label_of(t2)][a2_.weight_of(t2)]
          .emplace_back(a2_.dst_of(t2));
    }

    void clear()
    {
      dout1_.clear();
      dout2_.clear();
      nout1_.clear(); // FIXME: is nout1_ needed at all?
      nout2_.clear();
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
      , a1_state_no_(a1.num_all_states())
      , backtracker_(*this)
    {}

    // void play_with_queens()
    // {
    //   std::cerr << "Playing with the n-queens problem...\n";
    //   nqueens::mybacktracker m;
    //   nqueens::queens_t empty_solution; empty_solution.queen_no = 22;
    //   std::cerr << m(empty_solution) << "\n";
    // }

    const full_response
    get_full_response()
    {
      //play_with_queens();
      clear();

      // If we prove non-isomorphism at this point, it will be because
      // of sizes.
      full_response_.response = full_response::tag::trivially_different;

      require(is_accessible(a1_) && is_accessible(a2_),
              "are-isomorphic: input automata must both be accessible");

      // Before even initializing our data structures, which is
      // potentially expensive, try to simply compare the number of
      // elements such as states and transitions.
      if (trivially_different())
        return full_response_;

      // FIXME: return trivially_different if only one of the two
      // automata is sequential.
      if (are_automata_sequential_filling_douts())
        return get_full_response_sequential();
      else
        return get_full_response_nonsequential();
    }

    /// A mapping from an a1_ state to an a2_ state.
    using tentative_mapping_t = std::unordered_map<state1_t, state2_t>;

    /// A mapping from an a2_ state to an a1_ state.
    using tentative_reverse_mapping_t = std::unordered_map<state2_t, state1_t>;

    void make_isomorphic(state1_t s1,
                         state2_t s2,
                         tentative_mapping_t& tm,
                         tentative_reverse_mapping_t& tr)
    {
      tm[s1] = s2;
      tr[s2] = s1;
//      std::cerr << "+ ADD    <" << s1 << ", " << s2 << ">\n";
    }

    void make_nonisomorphic(state1_t s1,
                            state2_t s2,
                            tentative_mapping_t& tm,
                            tentative_reverse_mapping_t& tr)
    {
      tm.erase(s1);
      tr.erase(s2);
//      std::cerr << "+ REMOVE <" << s1 << ", " << s2 << ">\n";
    }

    bool are_isomorphic(state1_t s1,
                        state2_t s2,
                        const tentative_mapping_t& tm,
                        const tentative_reverse_mapping_t& tr)
    {
      const auto& is1 = tm.find(s1);
      return is1 != tm.cend() && is1->second == s2;
    }

    bool are_nonisomorphic(state1_t s1,
                           state2_t s2,
                           const tentative_mapping_t& tm,
                           const tentative_reverse_mapping_t& tr)
    {
      const auto& is1 = tm.find(s1);
      const auto& is2 = tr.find(s2);
      return (is1 != tm.cend() && is1->second != s2)
             || (is2 != tr.cend() && is2->second != s1);
    }

    class failure : public std::exception
    {};

# define FAIL                                    \
    throw failure()

    void find_isomorphism(state1_t s1,
                          state2_t s2,
                          tentative_mapping_t& tm,
                          tentative_reverse_mapping_t& tr)
    {
      if (are_isomorphic(s1, s2, tm, tr))
        {
//          std::cerr << "Tentatively ISOMORPHIC: " << s1 << " <-> " << s2 << "\n";
          return;
        }
      if (are_nonisomorphic(s1, s2, tm, tr))
        {
//          std::cerr << "Tentatively NOT isomorphic: " << s1 << " <-> " << s2 << "\n";
          FAIL;
        }

      // s1 and s2 are not isomorphic if their successor-set size is
      // different.  After excluding this possibility we'll be able to
      // perform a simpler "asymmetric" check simply verify that each
      // successor of s1 has an isomorphic successor of s2.
//      std::cerr << "Checking the successor no of " << s1 << " and " << s2 << ": ";
      if (nout1_[s1].size() != nout2_[s2].size())
        {
//          std::cerr << "\nNOT isomorphic because they have different successor no: " << s1 << ", " << s2 << "\n";
          FAIL;
        }
//      std::cerr << "it's the same (" << nout1_[s1].size() << ").  Going on.\n";

# define PRECHECK_OUT_TRANSITIONS
# ifdef PRECHECK_OUT_TRANSITIONS
      {
        // s1 and s2 are not isomorphic if they have a different set
        // of label and weights on their outgoing transitions.
        std::map<label_t, std::vector<weight_t>, vcsn::less<labelset_t>> lc1, lc2; // FIXME: would std::unordered_map's compare equal?
        for (auto t : a1_.all_out(s1)) // std::vector<bool> lacks emplace_back.
          lc1[a1_.label_of(t)].push_back(a1_.weight_of(t));
        for (auto& q : lc1)
          std::sort(q.second.begin(), q.second.end(), weightset_t::less_than);
        for (auto t : a2_.all_out(s2))
          lc2[a2_.label_of(t)].push_back(a2_.weight_of(t));
        for (auto& q : lc2)
          std::sort(q.second.begin(), q.second.end(), weightset_t::less_than);
        if (lc1 != lc2)
          FAIL;
      }
# endif // # ifdef PRECHECK_OUT_TRANSITIONS

      // FIXME: possible optimization idea: sort transitions by
      // inverse label frequency, so that we don't have to backtrack
      // over trivial decisions (particularly when a given label has
      // transition count 1).

      make_isomorphic(s1, s2, tm, tr);

# ifndef PRECHECK_OUT_TRANSITIONS
try{
# endif // # ifndef PRECHECK_OUT_TRANSITIONS
      // Check if there exists an isomorphism between successors of s1
      // and successors of s2.
      for (auto t1 : a1_.all_out(s1))
        {
          state_t ss1 = a1_.dst_of(t1);
          const label_t& l = a1_.label_of(t1);
          const weight_t& w = a1_.weight_of(t1);

          // s1 and s2 are not isomorphic if we find mo match for <l, ss1>
          // among the successors of s2.
//          std::cerr << "Looking at the successors of <" << s1 << ", " << s2 << "> with letter '";
//          a1_.context().labelset()->print(std::cerr, l);
//          std::cerr << "' and weight ";
//          a1_.context().weightset()->print(std::cerr, w);
//          std::cerr << " (an a2_ state matching " << ss1 << " in a1_):\n";
          for (auto ss2 : nout2_.at(s2).at(l).at(w))
            {
//              std::cerr << "[\n";
//              std::cerr << "Considering <" << ss1 << ", " << ss2 << ">\n";
              // tentative_mapping_t new_tm = tm;
              // tentative_reverse_mapping_t new_tr = tr;
              try
                {
                  find_isomorphism(ss1, ss2, /*new_*/tm, /*new_*/tr);
//                  std::cerr << "Cool, " << ss1 << " and " << ss2 << " ARE isomorphic in this context\n";
//                  std::cerr << "]\n";
                  goto out;
                }
              catch (/*...*/const failure&)
                {
//                  std::cerr << "No, " << ss1 << " and " << ss2 << " are NOT isomorphic in this context.  Trying next choices\n";
//                  std::cerr << "]\n";
                }
            } // inner for
//          std::cerr << "There's no isomorphism between the successors of " << s1 << " and " << s2 << "\n";
          make_nonisomorphic(s1, s2, tm, tr);
          std::cerr << "«backtrack»\n";
          FAIL;
        out:
          int useless __attribute__((unused)) = 42;
        } // outer for
//      std::cerr << "After examining successors, we found that <" << s1 << ", " << s2 << "> ARE isomorphic in this context\n";
# ifndef PRECHECK_OUT_TRANSITIONS
}
catch(...){ // FIXME: find the correct exception, after the Internet starts working again
//   std::cerr << "at() failed while examining the successors of " << s1 << ", " << s2 << ": those two are not isomorphic in this context\n";;
  make_nonisomorphic(s1, s2, tm, tr);
  FAIL;
}
# endif // # ifndef PRECHECK_OUT_TRANSITIONS
    }

# undef FAIL

    const full_response
    get_full_response_nonsequential()
    {
      isomorphism_t initial_solution = {s1tos2_t(), s2tos1_t(), false};
      try
        {
          // std::cerr << "OK-A 100\n";
          isomorphism_t solution =
            std::move(backtracker_(std::move(initial_solution)));
          // std::cerr << "OK-A 110\n";
          full_response_.response = full_response::tag::isomorphic;
          s1tos2_ = std::move(solution.s1tos2);
          s2tos1_ = std::move(solution.s2tos1);
        }
      catch (const std::out_of_range&)
        {
          // std::cerr << "OK-B\n";
          full_response_.response = full_response::tag::nocounterexample;
        }
      // std::cerr << "OK-C\n";
      return full_response_;;
    }

    // const full_response
    // get_full_response_nonsequential()
    // {
    //   std::cerr << "get_full_response_nonsequential\n";

    //   fill_nouts_();

    //   tentative_mapping_t tm;
    //   tentative_reverse_mapping_t tr;

    //   make_isomorphic(a1_.post(), a2_.post(), tm, tr);
    //   try
    //     {
    //       find_isomorphism(a1_.pre(), a2_.pre(), tm, tr);
    //       full_response_.response = full_response::tag::isomorphic;
    //     }
    //   catch (/*...*/const failure&)
    //     {
    //       full_response_.response = full_response::tag::nocounterexample;
    //     }
    //   //if (are_nonsequential_states_isomorphic(a1_.pre(), a2_.pre(), tm, tr))
    //   //else
    //   //std::cerr << "Are they isomorphic? " << (full_response_.response == full_response::tag::isomorphic) << "\n";
    //   //std::cerr << "\n";
    //   return full_response_;
    // }

    const full_response
    get_full_response_sequential()
    {
      std::cerr << "get_full_response_sequential\n";

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
          if (dout1_[s1].size() != dout2_[s2].size())
            return full_response_;

          for (const auto& l1_w1dst1 : dout1_[s1]) // dout1_.at(s1) may fail.
            {
              const label_t& l1 = l1_w1dst1.first;
              const weight_t& w1 = l1_w1dst1.second.first;
              const state_t& dst1 = l1_w1dst1.second.second;

              const auto& s2out = dout2_.find(s2);
              if (s2out == dout2_.cend())
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

      if (r.response == full_response::tag::counterexample)
        std::cerr << "COUNTEREXAMPLE: " << r.counterexample.first << ", " << r.counterexample.second << "\n";
      else if (r.response == full_response::tag::nocounterexample)
        std::cerr << "NO counterexample\n";
      else if (r.response == full_response::tag::isomorphic)
        std::cerr << "ISOMORPHIC\n";
      else
        std::cerr << "SOMETHING ELSE\n";

      return r.response == full_response::tag::isomorphic;
    }

    /// A map from each a2_ state to the corresponding a1_ state.
    using origins_t = std::map<state2_t, state1_t>;

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
