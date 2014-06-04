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

  namespace detail
  {
    template <typename Aut>
    class pair_automaton_impl
      : public automaton_decorator<
          typename Aut::element_type::automaton_nocv_t>
    {
    public:
      using automaton_t =  Aut;
      using automaton_nocv_t =
          typename automaton_t::element_type::automaton_nocv_t;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using super_t = automaton_decorator<automaton_nocv_t>;

    private:
      /// The semantics of the result states: ordered pair of input
      /// states.
      using pair_t = std::pair<state_t, state_t>;

    public:
      pair_automaton_impl(const automaton_t& aut, bool keep_initials = false)
        : super_t(aut->context())
        , input_(aut)
        , transition_map_(aut)
        , keep_initials_(keep_initials)
      {}

      static std::string sname()
      {
        return "pair_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "pair_automaton<" + input_->vname(full) + ">";
      }

      void pair()
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

        called_ = true;
      }

      const std::unordered_map<pair_t, state_t>& get_map_pair() const
      {
        require(called_,
                "trying to call get_map_pair() before calling pair()");
        return pair_states_;
      }

      state_t get_q0() const
      {
        require(!keep_initials_, "can't get_q0() on a pairer that "
                "keeps origins");
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

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(std::ostream& o, state_t ss,
                       const std::string& fmt = "text") const
      {
        auto ps = origins().at(ss);
        input_->print_state_name(o, ps.first, fmt);
        o << ", ";
        input_->print_state_name(o, ps.second, fmt);
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
      state_t q0_;
      bool called_ = false;
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
    res->pair();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut, typename>
      automaton
      pair(const automaton& aut, bool keep_initials = false)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(pair(a, keep_initials));
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
      automaton_t aut_;
      pair_automaton<Aut> pair_;
      std::unordered_map<state_t, std::pair<unsigned, transition_t>> paths_;
      std::unordered_set<state_t> todo_;
      state_t q0_;
      word_t res_;

    public:
      synchronizer(const automaton_t& aut)
        : aut_(aut)
        {}

    private:
      void init_pair()
      {
        pair_ = pair(aut_);
        q0_ = pair_->get_q0();
        paths_ = paths_ibfs(pair_, q0_);
      }

      void init_synchro()
      {
        init_pair();
        require(pair_->states().size() == paths_.size() + 1,
                "automaton is not synchronizing");
        for (auto s : pair_->states())
          if (s != q0_)
            todo_.insert(s);
      }

      std::vector<transition_t> recompose_path(state_t from)
      {
        std::vector<transition_t> res;
        state_t bt_curr = from;
        while (bt_curr != q0_)
          {
            transition_t t = paths_.find(bt_curr)->second.second;
            res.push_back(t);
            bt_curr = pair_->dst_of(t);
          }
        return res;
      }

      int dist(state_t s)
      {
        if (s == q0_)
          return 0;
        return paths_.find(s)->second.first;
      }

      void apply_label(const label_t& label)
      {
        res_ = aut_->labelset()->concat(res_, label);
        std::unordered_set<state_t> new_todo;
        for (auto s : todo_)
          {
            auto ntf = pair_->out(s, label);
            auto size = ntf.size();
            require(0 < size, "automaton must be complete");
            require(size < 2, "automaton must be deterministic");
            auto new_state = pair_->dst_of(*ntf.begin());
            if (new_state != q0_)
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

      word_t synchroP()
      {
        return synchro(&synchronizer::phi_1);
      }

      word_t synchroPL()
      {
        return synchro(&synchronizer::phi_2);
      }

    private:
      word_t synchro(int (synchronizer::*heuristic)(state_t))
      {
        init_synchro();
        while (0 < todo_.size())
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

      int delta(state_t p, std::vector<transition_t> w)
      {
        state_t np = p;
        for (auto t : w)
          {
            auto l = pair_->label_of(t);
            auto ntf = pair_->out(np, l);
            auto size = ntf.size();
            require(0 < size, "automaton must be complete");
            require(size < 2, "automaton must be deterministic");
            np = pair_->dst_of(*ntf.begin());
          }

        return dist(np) - dist(p);
      }

      int phi_1(state_t p)
      {
        int res = 0;
        auto w = recompose_path(p);
        for (auto s: todo_)
          if (s != p)
            res += delta(s, w);
        return res;
      }

      int phi_2(state_t p)
      {
          return phi_1(p) + dist(p);
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
    else if (boost::iequals(algo, "synchrop"))
      return sync.synchroP();
    else if (boost::iequals(algo, "synchropl"))
      return sync.synchroPL();
    else
      raise("synchronizing_word: invalid algorithm: ", str_escape(algo));
  }

  namespace dyn
  {
    namespace detail
    {
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
}

#endif // !VCSN_ALGOS_SYNCHRONIZING_WORD_HH
