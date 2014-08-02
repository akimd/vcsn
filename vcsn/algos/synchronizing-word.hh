#ifndef VCSN_ALGOS_SYNCHRONIZING_WORD_HH
# define VCSN_ALGOS_SYNCHRONIZING_WORD_HH

# include <algorithm>
# include <iostream>
# include <limits>
# include <map>
# include <queue>
# include <set>
# include <unordered_set>
# include <utility>
# include <vector>

# include <boost/algorithm/string.hpp>

# include <vcsn/algos/distance.hh>
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/transition-map.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/label.hh>
# include <vcsn/misc/map.hh>
# include <vcsn/misc/pair.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/zip-maps.hh>

namespace vcsn
{

  /*--------------------------------------.
  | is_synchronized_by(automaton, word).  |
  `--------------------------------------*/

  template <typename Aut>
  bool
  is_synchronized_by(const Aut& aut,
                     const typename labelset_t_of<Aut>::word_t& w)
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;

    std::unordered_set<state_t> todo;

    for (auto s : aut->states())
      todo.insert(s);

    for (auto l : aut->labelset()->letters_of(w))
      {
        std::unordered_set<state_t> new_todo;
        for (auto s : todo)
          {
            auto ntf = aut->out(s, l);
            auto size = ntf.size();
            require(0 < size, "automaton must be complete");
            require(size < 2, "automaton must be deterministic");
            new_todo.insert(aut->dst_of(*ntf.begin()));
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


  /*-----------------.
  | pair_automaton.  |
  `-----------------*/

  namespace detail
  {
    template <typename Aut>
    class pair_automaton_impl
      : public automaton_decorator<mutable_automaton<context_t_of<Aut>>>
    {
    public:
      using automaton_t =  Aut;
      using automaton_nocv_t = mutable_automaton<context_t_of<Aut>>;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using super_t = automaton_decorator<automaton_nocv_t>;

    private:
      /// The semantics of the result states: ordered pair of input
      /// states.
      using pair_t = std::pair<state_t, state_t>;
      using origins_t = std::map<state_t, pair_t>;

    public:
      pair_automaton_impl(const automaton_t& aut, bool keep_initials = false)
        : super_t(aut->context())
        , input_(aut)
        , transition_map_(aut)
        , keep_initials_(keep_initials)
      {
        auto ctx = input_->context();
        auto ws = ctx.weightset();

        if (!keep_initials_)
          {
            q0_ = this->new_state(); // q0 special state
            for (auto l : input_->labelset()->genset())
              this->add_transition(q0_, q0_, l, ws->one());
          }
        else
          for (auto s : input_->states())
            pair_states_.emplace(std::make_pair(s, s), this->new_state());

        // States are "ordered": (s1, s2) is defined only for s1 < s2.
        {
          auto states = input_->states();
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
                                     this->new_state());
            }
        }

        for (auto ps : pair_states_)
          {
            auto pstates = ps.first; // pair of states
            auto cstate = ps.second; // current state

            for (const auto& p : zip_maps(transition_map_[pstates.first],
                                          transition_map_[pstates.second]))
              this->add_transition(cstate,
                                  state_(std::get<0>(p.second).dst,
                                         std::get<1>(p.second).dst),
                                  p.first, ws->one());
          }

        for (const auto& p: pair_states_)
          origins_.emplace(p.second, p.first);

        if (keep_initials_)
          for (auto s : input_->states())
            singletons_.push_back(state_(s, s));
        else
          singletons_.push_back(q0_);
      }

      state_t get_q0() const
      {
        require(!keep_initials_,
                "can't get_q0() on a pairer that keeps origins");
        return q0_;
      }

      bool is_singleton(state_t s) const
      {
        if (keep_initials_)
          {
            pair_t p = get_origin(s);
            return p.first == p.second;
          }
        else
          return s == q0_;
      }

      const std::vector<state_t>& singletons()
      {
        return singletons_;
      }

      static std::string sname()
      {
        return "pair_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "pair_automaton<" + input_->vname(full) + ">";
      }

      const std::unordered_map<pair_t, state_t>& get_map_pair() const
      {
        return pair_states_;
      }

      /// A map from result state to tuple of original states.
      const origins_t& origins() const
      {
        return origins_;
      }

      const pair_t get_origin(state_t s) const
      {
        auto i = origins().find(s);
        require(i != std::end(origins()), "state not found in origins");
        return i->second;
      }

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(state_t ss, std::ostream& o,
                       const std::string& fmt = "text") const
      {
        auto i = origins().find(ss);
        if (i == std::end(origins()))
          this->print_state(ss, o);
        else
          {
            input_->print_state_name(i->second.first, o, fmt);
            o << ", ";
            input_->print_state_name(i->second.second, o, fmt);
          }
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
        if (s1 == s2 && !keep_initials_)
          return q0_;
        else
          return pair_states_[make_ordered_pair(s1, s2)];
      }

      /// Input automaton.
      automaton_t input_;
      /// Fast maps label -> (weight, label).
      using transition_map_t
        = transition_map<automaton_t, weightset_t_of<automaton_t>, true>;
      transition_map_t transition_map_;
      std::unordered_map<pair_t, state_t> pair_states_;
      origins_t origins_;
      std::vector<state_t> singletons_;
      state_t q0_;
      bool keep_initials_ = false;
    };
  }

  template <typename Aut>
  using pair_automaton
    = std::shared_ptr<detail::pair_automaton_impl<Aut>>;

  /*------------------.
  | pair(automaton).  |
  `------------------*/

  template <typename Aut>
  pair_automaton<Aut> pair(const Aut& aut, bool keep_initials = false)
  {
    auto res = make_shared_ptr<pair_automaton<Aut>>(aut, keep_initials);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename>
      automaton
      pair(const automaton& aut, bool keep_initials = false)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::pair(a, keep_initials));
      }

      REGISTER_DECLARE(pair, (const automaton&, bool) -> automaton);
    }
  }


  namespace detail
  {
    template <typename Aut>
    class synchronizer
    {
    public:
      using automaton_t = Aut;
      using word_t = typename labelset_t_of<automaton_t>::word_t;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

    private:
      using pair_t = std::pair<state_t, state_t>;
      using dist_transition_t = std::pair<unsigned, transition_t>;

      automaton_t aut_;
      pair_automaton<Aut> pair_;
      std::unordered_map<state_t, dist_transition_t> paths_;
      std::unordered_set<state_t> todo_;
      word_t res_;

    public:
      synchronizer(const automaton_t& aut)
        : aut_(aut)
        {}

    private:
      void init_pair(bool keep_initials = false)
      {
        pair_ = pair(aut_, keep_initials);
        paths_ = paths_ibfs(pair_, pair_->singletons());

        if (keep_initials)
          for (auto it = paths_.begin(); it != paths_.end(); /* nothing */)
            {
              if (pair_->is_singleton(it->first))
                paths_.erase(it++);
              else
                ++it;
            }
      }

      void init_synchro(bool keep_initials = false)
      {
        init_pair(keep_initials);
        require(pair_->states().size()
                == paths_.size() + pair_->singletons().size(),
                "automaton is not synchronizing");

        for (auto s : pair_->states())
          if (!pair_->is_singleton(s))
            todo_.insert(s);
      }

      std::vector<transition_t> recompose_path(state_t from)
      {
        std::vector<transition_t> res;
        state_t bt_curr = from;
        while (!pair_->is_singleton(bt_curr))
          {
            transition_t t = paths_.find(bt_curr)->second.second;
            res.push_back(t);
            bt_curr = pair_->dst_of(t);
          }
        return res;
      }

      int dist(state_t s)
      {
        if (pair_->is_singleton(s))
          return 0;
        return paths_.find(s)->second.first;
      }

      state_t dest_state(state_t s, const label_t& l)
      {
        auto ntf = pair_->out(s, l);
        auto size = ntf.size();
        require(0 < size, "automaton must be complete");
        require(size < 2, "automaton must be deterministic");
        return pair_->dst_of(*ntf.begin());
      }

      void apply_label(const label_t& label)
      {
        res_ = aut_->labelset()->concat(res_, label);
        std::unordered_set<state_t> new_todo;
        for (auto s : todo_)
          {
            auto new_state = dest_state(s, label);
            if (!pair_->is_singleton(new_state))
              new_todo.insert(new_state);
          }
        todo_ = std::move(new_todo);
      }

      // "Apply" a word to the set of active states (for each state, for each
      // label, perform s = d(s))
      void apply_path(const std::vector<transition_t>& path)
      {
        for (auto t : path)
          apply_label(pair_->label_of(t));
      }

    public:

      // We just perform an inverse BFS from q0 and put all the accessible
      // states in 'paths'. If the size of paths is the same than the number
      // of states of pa (minus q0), then for each pair of states (p, q),
      // there is a word w such that d(p, w) = d(q, w), thus the automaton is
      // synchronizing.
      bool is_synchronizing()
      {
        init_pair();
        return paths_.size() == pair_->states().size() - 1;
      }

      word_t greedy()
      {
        return synchro(&synchronizer::dist);
      }

      word_t cycle()
      {
        return cycle_();
      }

      word_t synchroP()
      {
        return synchro(&synchronizer::phi_1);
      }

      word_t synchroPL()
      {
        return synchro(&synchronizer::phi_2);
      }

      word_t fastsynchro()
      {
        return fastsynchro_();
      }

    private:
      using heuristic_t = auto (synchronizer::*)(state_t) -> int;

      word_t synchro(heuristic_t heuristic)
      {
        init_synchro();
        while (!todo_.empty())
          {
            int min = std::numeric_limits<int>::max();
            state_t s_min = 0;
            for (auto s : todo_)
              {
                int d = (this->*(heuristic))(s);
                if (d < min)
                  {
                    min = d;
                    s_min = s;
                  }
              }

            apply_path(recompose_path(s_min));
          }
        return res_;
      }

      word_t cycle_()
      {
        init_synchro(true);
        bool first = true;
        state_t previous = 0;
        while (!todo_.empty())
          {
            int min = std::numeric_limits<int>::max();
            state_t s_min = 0;
            for (auto s : todo_)
              {
                pair_t o = pair_->get_origin(s);
                if (!first && o.first != previous && o.second != previous)
                  continue;
                int d = dist(s);
                if (d < min)
                  {
                    min = d;
                    s_min = s;
                  }
              }

            const auto& path = recompose_path(s_min);
            pair_t pair_end = pair_->get_origin(
                    pair_->dst_of(path[path.size() - 1]));
            assert(pair_end.first == pair_end.second);
            previous = pair_end.first;
            first = false;
            apply_path(path);
          }
        return res_;
      }

      word_t fastsynchro_()
      {
        init_synchro();

        // The drawback of this algorithm is that it does not guarantee us to
        // converge, so we this to counter prevent potential infinite loops.
        unsigned count = 0;
        while (!todo_.empty())
          {
            // compute lmin = arg min { phi_3(l) } forall l in labelset
            label_t lmin;
            int min = std::numeric_limits<int>::max();
            for (const auto& l : pair_->labelset()->genset())
              {
                int cur_min = phi_3(l);
                if (cur_min < min)
                  {
                    min = cur_min;
                    lmin = l;
                  }
              }

            unsigned sq_bound = aut_->states().size();
            if (min < 0 && count++ < (sq_bound * sq_bound))
              apply_label(lmin);
            else
              {
                // fallback on the phi_2 heuristic, with a size restriction.
                int count = 0;
                size_t t = todo_.size();
                int bound = std::min(aut_->states().size(), (t * t - t / 2));
                int min = std::numeric_limits<int>::max();
                state_t s_min = 0;
                for (auto s : todo_)
                  {
                    if (count++ >= bound)
                      break;
                    int d = phi_2(s);
                    if (d < min)
                      {
                        min = d;
                        s_min = s;
                      }
                  }
                apply_path(recompose_path(s_min));
              }
          }
        return res_;
      }

      /// Compute dist(d(s, w)) - dist(s).
      int delta(state_t p, const std::vector<transition_t>& w)
      {
        state_t np = p;
        for (auto t : w)
          np = dest_state(np, pair_->label_of(t));
        return dist(np) - dist(p);
      }

      /// Heuristic used for SynchroP.
      /// phi_1(p) = Sum delta(s, w) for all s in the active states, with
      /// s != p. w is the shortest word that syncs the pair p.
      int phi_1(state_t p)
      {
        int res = 0;
        auto w = recompose_path(p);
        for (auto s: todo_)
          if (s != p)
            res += delta(s, w);
        return res;
      }

      /// Heuristic used for SynchroPL.
      /// phi2(p) = phi_1(p).
      int phi_2(state_t p)
      {
        return phi_1(p) + dist(p);
      }

      /// Heuristic used for FastSynchro.
      /// phi3(l) = Sum dist(d(s, l)) - dist(s) forall s in the active states.
      int phi_3(const label_t& l)
      {
        int res = 0;
        for (auto s: todo_)
          res += dist(dest_state(s, l)) - dist(s);
        return res;
      }
    };
  }


  /*-----------------------------.
  | is_synchronizing(automaton). |
  `-----------------------------*/

  template <typename Aut>
  bool is_synchronizing(const Aut& aut)
  {
    vcsn::detail::synchronizer<Aut> sync(aut);
    return sync.is_synchronizing();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool is_synchronizing(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return vcsn::is_synchronizing(a);
      }

      REGISTER_DECLARE(is_synchronizing, (const automaton&) -> bool);
    }
  }


  /*-------------------------------.
  | synchronizing_word(automaton). |
  `-------------------------------*/

  template <typename Aut>
  typename labelset_t_of<Aut>::word_t
  synchronizing_word(const Aut& aut, const std::string& algo = "greedy")
  {
    vcsn::detail::synchronizer<Aut> sync(aut);
    if (boost::iequals(algo, "greedy") || boost::iequals(algo, "eppstein"))
      return sync.greedy();
    else if (boost::iequals(algo, "cycle"))
      return sync.cycle();
    else if (boost::iequals(algo, "synchrop"))
      return sync.synchroP();
    else if (boost::iequals(algo, "synchropl"))
      return sync.synchroPL();
    else if (boost::iequals(algo, "fastsynchro"))
      return sync.fastsynchro();
    else
      raise("synchronizing_word: invalid algorithm: ", str_escape(algo));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      label
      synchronizing_word(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        auto word = vcsn::synchronizing_word(a, algo);
        return make_label(make_wordset(*a->labelset()), word);
      }

      REGISTER_DECLARE(synchronizing_word,
                       (const automaton&, const std::string&) -> label);
    }
  }

  /*--------.
  | cerny.  |
  `--------*/

  /// Cerny automata are automata whose synchronizing word length is always
  /// (n - 1)^2, the upper bound of the Cerny's conjecture.
  ///
  /// Their transition function d(q, l) is defined by:
  ///
  /// - (q + 1) % n  if l == a
  /// - q            if l != a and q != n - 1
  /// - 0            if l != a and q == n - 1

  template <typename Ctx>
  mutable_automaton<Ctx>
  cerny(const Ctx& ctx, unsigned num_states)
  {
    require(0 < num_states, "num_states must be > 0");

    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    std::vector<state_t> states;
    states.reserve(num_states);

    for (unsigned i = 0; i < num_states; ++i)
      states.push_back(res->new_state());

    for (unsigned i = 0; i < num_states; ++i)
      {
        bool la = true;
        for (auto l : ctx.labelset()->genset())
          {
            auto dest = (la || i == num_states - 1) ? (i + 1) % num_states : i;
            res->add_transition(states[i], states[dest], l,
                                ctx.weightset()->one());
            la = false;
          }
      }

    res->set_initial(states[0]);
    res->set_final(states[0]);

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename>
      automaton
      cerny(const context& ctx, unsigned num_states)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(vcsn::cerny(c, num_states));
      }

      REGISTER_DECLARE(cerny,
                       (const context& ctx, unsigned n) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_SYNCHRONIZING_WORD_HH
