#ifndef VCSN_ALGOS_SYNCHRONIZING_WORD_HH
# define VCSN_ALGOS_SYNCHRONIZING_WORD_HH

# include <algorithm>
# include <iostream>
# include <map>
# include <queue>
# include <set>
# include <unordered_set>
# include <utility>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/distance.hh>
# include <vcsn/algos/product.hh> // transition_map
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/label.hh>
# include <vcsn/misc/pair.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*--------------------------------------.
  | is_synchronized_by(automaton, word).  |
  `--------------------------------------*/

  template <typename Aut>
  bool
  is_synchronized_by(const Aut& aut,
                     const typename Aut::labelset_t::word_t& w)
  {
    using context_t = typename Aut::context_t;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;

    std::unordered_set<state_t> todo;

    for (auto s : aut.states())
      todo.insert(s);

    for (auto l : aut.labelset()->letters_of(w))
      {
        std::unordered_set<state_t> new_todo;
        for (auto s : todo)
          {
            auto ntf = aut.out(s, l);
            auto size = ntf.size();
            require(0 < size, "automaton must be complete");
            require(size < 2, "automaton must be deterministic");
            new_todo.insert(aut.dst_of(*ntf.begin()));
          }
        todo = std::move(new_todo);
      }

    return todo.size() == 1;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename LabelSet>
      bool
      is_synchronized_by(const automaton& aut, const label& word)
      {
        const auto& a = aut->as<Aut>();
        const auto& w = word->as<LabelSet>();
        return vcsn::is_synchronized_by(a, w.label());
      }

      REGISTER_DECLARE(is_synchronized_by,
                       (const automaton&, const label&) -> bool);
    }
  }

  namespace detail
  {
    template <typename Aut>
    class pairer
    {
    public:
      using context_t = typename Aut::context_t;
      using automaton_t =  mutable_automaton<context_t>;
      using state_t = typename automaton_t::state_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

    private:
      /// The semantics of the result states: ordered pair of input
      /// states.
      using pair_t = std::pair<state_t, state_t>;

    public:
      pairer(const automaton_t& aut)
        : aut_(aut)
        , res_(aut.context())
        , transition_map_(aut)
      {}

      automaton_t pair()
      {
        auto ctx = aut_.context();
        auto ws = ctx.weightset();

        q0_ = res_.new_state(); // q0 special state

        for (auto l : aut_.labelset()->genset())
          res_.add_transition(q0_, q0_, l, ws->one());

        // States are "ordered": (s1, s2) is defined only for s1 < s2.
        {
          auto states = aut_.states();
          auto end = std::end(states);
          for (auto i1 = std::begin(states); i1 != end; ++i1)
            {
              // FIXME: cannot use i2 = std::next(i1) with clang 3.5
              // and Boost 1.55.
              // https://svn.boost.org/trac/boost/ticket/9984
              auto i2 = i1;
              for (++i2; i2 != end; ++i2)
                // s1 < s2, no need for make_ordered_pair.
                pair_states_.emplace(std::make_pair(*i1, *i2),
                                     res_.new_state());
            }
        }

        for (auto ps : pair_states_)
          {
            auto pstates = ps.first; // pair of states
            auto cstate = ps.second; // current state

            for (const auto& p : zip_maps(transition_map_[pstates.first],
                                          transition_map_[pstates.second]))
              res_.add_transition(cstate,
                                  state_(std::get<0>(p.second).dst,
                                         std::get<1>(p.second).dst),
                                  p.first,
                                  // FIXME: Adding weights is really suspicious.
                                  ws->add(std::get<0>(p.second).wgt,
                                          std::get<1>(p.second).wgt));
          }

        called_ = true;
        return std::move(res_);
      }

      const std::unordered_map<pair_t, state_t>& get_map_pair() const
      {
        require(called_,
                "trying to call get_map_pair() before calling pair()");
        return pair_states_;
      }

      state_t get_q0() const
      {
        require(called_, "trying to call get_q0() before calling pair()");
        return q0_;
      }

      /// A map from result state to tuple of original states.
      using origins_t = std::map<state_t, pair_t>;

      /// A map from result state to tuple of original states.
      origins_t origins() const
      {
        origins_t res;
        for (const auto& p: pair_states_)
          res.emplace(p.second, p.first);
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n"
             "    0 [label = \"q0\"]\n";
        for (auto p: orig)
          if (p.first != automaton_t::pre() && p.first != automaton_t::post())
            o << "    " << p.first - 2
              << " [label = \""
              << p.second.first - 2 << ", " << p.second.second - 2
              << "\"]\n";
        o << "*/\n";
        return o;
      }

    private:
      /// The state in the result automaton that corresponds to (s1,
      /// s2).  Allocate it if needed.
      state_t state_(state_t s1, state_t s2)
      {
        // Benches show it is slightly faster to handle this case
        // especially rather that mapping these "diagonal states" to
        // q0_ in pair_states_.
        if (s1 == s2)
          return q0_;
        else
          return pair_states_[make_ordered_pair(s1, s2)];
      }

      /// Input automaton.
      const automaton_t& aut_;
      /// Result.
      automaton_t res_;
      /// Fast maps label -> (weight, label).
      using transition_map_t
        = transition_map<automaton_t, typename automaton_t::weightset_t, true>;
      transition_map_t transition_map_;
      std::unordered_map<pair_t, state_t> pair_states_;
      state_t q0_;
      bool called_ = false;
    };
  }

  /*------------------.
  | pair(automaton).  |
  `------------------*/

  template <typename Aut>
  Aut pair(const Aut& aut)
  {
    detail::pairer<Aut> pair(aut);
    auto res = pair.pair();
    if (getenv("VCSN_ORIGINS"))
      pair.print(std::cout, pair.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      pair(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(pair(a));
      }

      REGISTER_DECLARE(pair, (const automaton&) -> automaton);
    }
  }

  template <typename Aut>
  typename Aut::labelset_t::word_t
  synchronizing_word(const Aut& aut)
  {
    using automaton_t = Aut;
    using word_t = typename automaton_t::labelset_t::word_t;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;

    word_t res;
    std::unordered_set<state_t> todo;

    detail::pairer<Aut> pobj(aut);
    Aut pa = pobj.pair();
    state_t q0 = pobj.get_q0();

    std::unordered_map<state_t, std::pair<state_t, transition_t>> paths =
        paths_ibfs(pa, q0);

    if (paths.size() < pa.states().size() - 1)
        raise("automaton is not synchronizing.");

    for (auto s : pa.states())
      todo.insert(s);

    while (1 < todo.size() || todo.find(q0) == todo.end())
      {
        int min = -1;
        std::vector<transition_t> path;
        for (auto s : todo)
          if (s != q0)
            {
              std::vector<transition_t> cpath;
              state_t bt_curr = s;
              while (bt_curr != q0)
                {
                  transition_t t;
                  std::tie(bt_curr, t) = paths.find(bt_curr)->second;
                  cpath.push_back(t);
                }

              if (min == -1 || min > static_cast<long>(cpath.size()))
                {
                  min = cpath.size();
                  path = cpath;
                }
            }
        for (auto t : path)
          {
            auto l = pa.label_of(t);
            std::unordered_set<state_t> new_todo;
            for (auto s : todo)
              {
                auto ntf = pa.out(s, l);
                if (ntf.empty())
                  raise("automaton must be complete");
                if (1 < ntf.size())
                  raise("automaton must be deterministic");
                new_todo.insert(pa.dst_of(*ntf.begin()));
              }
            todo = std::move(new_todo);
            res = aut.labelset()->concat(res, l);
          }
      }

    return res;
  }

  /*-------------------------------.
  | synchronizing_word(automaton). |
  `-------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      label
      synchronizing_word(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_label(make_wordset(*a.labelset()), vcsn::synchronizing_word(a));
      }

      REGISTER_DECLARE(synchronizing_word, (const automaton&) -> label);
    }
  }
}

#endif // !VCSN_ALGOS_SYNCHRONIZING_WORD_HH
