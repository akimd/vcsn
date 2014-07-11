#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <stack>
# include <string>
# include <type_traits>
# include <map>
# include <unordered_map>
# include <vector>
# include <queue>

# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>
# include <vcsn/misc/map.hh> // vcsn::has
# include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*----------------------.
  | subset construction.  |
  `----------------------*/
  namespace detail
  {
    /// \brief The subset construction automaton from another.
    ///
    /// \tparam Aut an automaton type.
    /// \precondition labelset is free.
    /// \precondition weightset is Boolean.
    template <typename Aut>
    class determinized_automaton_impl
      : public automaton_decorator<mutable_automaton<context_t_of<Aut>>>
    // : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: requires free labelset");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "determinize: requires Boolean weights");

    public:
      using automaton_t = Aut;
      using automaton_nocv_t = mutable_automaton<context_t_of<Aut>>;
      // using automaton_nocv_t = typename automaton_t::element_type::automaton_nocv_t;
      using label_t = label_t_of<automaton_t>;
      using super_t = automaton_decorator<automaton_nocv_t>;

      /// Set of (input) states.
      using state_set = dynamic_bitset;

      /// Result automaton state type.
      using state_t = state_t_of<automaton_t>;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      determinized_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
        , finals_(state_size_)
      {
        // Input final states.
        for (auto t : input_->final_transitions())
          finals_.set(input_->src_of(t));

        // The input initial states.
        //
        // We could start with pre only, but then on an input
        // automaton without initial state, we would produce an empty
        // automaton (no states).  This would not conform to Jacques'
        // definition of determinization.
        state_set next;
        next.resize(state_size_);
        for (auto t : input_->initial_transitions())
          next.set(input_->dst_of(t));
        // Also pushes the initial state in the todo.
        this->set_initial(state(next));
      }

      static std::string sname()
      {
        return "determinized_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "determinized_automaton<" + input_->vname(full) + ">";
      }

      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_set& ss)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(ss);
        if (i == std::end(map_))
          {
            res = this->new_state();
            map_[ss] = res;

            if (ss.intersects(finals_))
              this->set_final(res);

            todo_.push(ss);
          }
        else
          res = i->second;
        return res;
      }

      /// Determinize all accessible states.
      void operator()()
      {
        std::map<label_t, state_set> ml;
        while (!todo_.empty())
          {
            auto ss = std::move(todo_.top());
            state_t src = state(ss);
            todo_.pop();

            ml.clear();
            for (auto s = ss.find_first(); s != ss.npos;
                 s = ss.find_next(s))
              {
                // Cache the output transitions of state s.
                auto i = successors_.find(s);
                if (i == successors_.end())
                  {
                    i = successors_.emplace(s, label_map_t{}).first;
                    auto& j = i->second;
                    for (auto t : input_->out(s))
                      {
                        auto l = input_->label_of(t);
                        if (j.find(l) == j.end())
                          j[l].resize(state_size_);
                        j[l].set(input_->dst_of(t));
                      }
                  }

                // Store in ml the possible destination per label.
                for (const auto& p : i->second)
                  {
                    auto j = ml.find(p.first);
                    if (j == ml.end())
                      ml[p.first] = p.second;
                    else
                      j->second |= p.second;
                  }
              }

            // Outgoing transitions from the current (result) state.
            for (const auto& e : ml)
              this->new_transition(src, state(e.second), e.first);
          }
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
            const char* sep = "";
            for (auto s: i->second)
              {
                o << sep;
                input_->print_state_name(s, o, fmt);
                sep = ", ";
              }
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      mutable origins_t origins_;

      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            {
              std::set<state_t> from;
              const auto& ss = p.first;
              for (auto s = ss.find_first(); s != ss.npos;
                   s = ss.find_next(s))
                from.emplace(s);
              origins_.emplace(p.second, std::move(from));
            }
        return origins_;
      }

    private:
      /// Set of input states -> output state.
      using map = std::unordered_map<state_set, state_t>;
      map map_;

      /// Input automaton.
      automaton_t input_;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;

      /// The sets of (input) states waiting to be processed.
      using stack = std::stack<state_set>;
      stack todo_;

      /// Set of final states in the input automaton.
      state_set finals_;

      /// successors[SOURCE-STATE][LABEL] = DEST-STATESET.
      using label_map_t = std::unordered_map<label_t, state_set>;
      using successors_t = std::unordered_map<state_t, label_map_t>;
      successors_t successors_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <typename Aut>
  using determinized_automaton
    = std::shared_ptr<detail::determinized_automaton_impl<Aut>>;

  template <typename Aut>
  inline
  auto
  determinize(const Aut& a)
    -> determinized_automaton<Aut>
  {
    auto res = make_shared_ptr<determinized_automaton<Aut>>(a);
    // Determinize.
    res->operator()();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      determinize(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::determinize(a));
      }

      REGISTER_DECLARE(determinize,
                       (const automaton& aut) -> automaton);
    }
  }


  /*---------------------------.
  | weighted determinization.  |
  `---------------------------*/
  namespace detail
  {
    /// \brief The weighted determinization of weighted automaton.
    ///
    /// \tparam Aut an weighted automaton type.
    /// \precondition labelset is free.
    template <typename Aut>
    class detweighted_automaton_impl
      : public automaton_decorator<mutable_automaton<context_t_of<Aut>>>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: requires free labelset");

    public:
      using automaton_t = Aut;
      using automaton_nocv_t = mutable_automaton<context_t_of<Aut>>;
      using super_t = automaton_decorator<automaton_nocv_t>;

      using label_t = label_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      /// An output state is a list of weighted input states.
      using state_name_t = std::map<state_t, weight_t>;

      /// Build the weighted determinizer.
      /// \param a         the weighted automaton to determinize
      detweighted_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
      {}

      static std::string sname()
      {
        return "detweighted_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "detweighted_automaton<" + input_->vname(full) + ">";
      }

      // Initialize initial state of new weighted automaton.
      void init_initial_state()
      {
        state_name_t ss;
        for (auto t : input_->initial_transitions())
          ss.emplace(input_->dst_of(t), input_->weight_of(t));
        this->set_initial(state_(ss));
      }

      /// The determinization of weighted automaton
      /// with the idea based on Mohri's algorithm.
      void operator()()
      {
        init_initial_state();

        // Store set of state destination by (state, weight) of each label.
        std::unordered_map<label_t, state_name_t,
                           vcsn::hash<labelset_t_of<automaton_t>>,
                           vcsn::equal_to<labelset_t_of<automaton_t>>> mls2w;

        // Store the weights of each label.
        std::unordered_map<label_t, weight_t,
                           vcsn::hash<labelset_t_of<automaton_t>>,
                           vcsn::equal_to<labelset_t_of<automaton_t>>> mlw;
        while (!todo_.empty())
          {
            auto ss = std::move(todo_.front());
            todo_.pop();
            auto src = map_[ss];

            mls2w.clear();
            mlw.clear();
            for (const auto& p : ss)
              {
                auto s = p.first;
                auto v = p.second;
                for (auto t : input_->out(s))
                  {
                    auto l = input_->label_of(t);
                    auto dst = input_->dst_of(t);
                    auto w = ws_.mul(v, input_->weight_of(t));

                    // Calculate weight on each trasition and
                    // update to the map mls2w with (state, weight)
                    if (mls2w.find(l) == mls2w.end())
                      {
                        mls2w[l][dst] = w;
                        mlw[l] = w;
                      }
                    else if (mls2w[l].find(dst) == mls2w[l].end())
                      {
                        mls2w[l][dst] = w;
                        mlw[l] = ws_.add(mlw[l], w);
                      }
                    else
                      {
                        mls2w[l][dst] = ws_.add(mls2w[l][dst], w);
                        mlw[l] = ws_.add(mlw[l], w);
                      }
                  }
              }

            for (auto& e : mls2w)
              {
                auto l = e.first;
                // The semiring the mul distribute over plus
                // so (vi*wi / w) + (vj*wj/w) = (vi*wi + vj*wj)/w.
                for (auto& f : e.second)
                  f.second = ws_.rdiv(f.second, mlw[l]);
                this->new_transition(src, state_(e.second), l, mlw[l]);
              }
          }
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
        if (i == origins().end())
          this->print_state(ss, o);
        else
          {
            const char* sep = "";
            for (auto s : i->second)
              {
                o << sep;
                sep = ", ";
                print_weight(ws_, s.second, o, fmt);
                input_->print_state_name(s.first, o, fmt);
              }
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }

    private:
      /// Compare two output states.
      struct state_name_less
      {
        bool operator()(const state_name_t& m1,
                        const state_name_t& m2) const
        {
          auto pm1 = m1.cbegin();
          auto pm2 = m2.cbegin();
          while (pm1 != m1.cend() && pm2 != m2.cend())
          {
            if (pm1->first < pm2->first)
              return true;
            if (pm2->first < pm1->first)
              return false;

            if (weightset_t::less_than(pm1->second, pm2->second))
               return true;
            if (weightset_t::less_than(pm2->second, pm1->second))
               return false;

            pm1++; pm2++;
          }

          return pm1 == m1.cend() && pm2 != m2.cend();
        }
      };

      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state_(const state_name_t& name)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(name);
        if (i == std::end(map_))
          {
            res = this->new_state();
            map_[name] = res;

            // TODOs: Improve finding the final state
            for (const auto& p : name)
              if (input_->is_final(p.first))
                this->add_final(res,
                                ws_.mul(p.second,
                                        input_->get_final_weight(p.first)));

            todo_.push(name);
          }
        else
          res = i->second;
        return res;
      };

      /// Map from state name to state number.
      std::map<state_name_t, state_t, state_name_less> map_;

      /// Input automaton.
      automaton_t input_;

      /// Its weightset.
      weightset_t ws_ = *input_->weightset();

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;

      /// The sets of (input) states waiting to be processed.
      using queue = std::queue<state_name_t>;
      queue todo_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <typename Aut>
  using detweighted_automaton
    = std::shared_ptr<detail::detweighted_automaton_impl<Aut>>;

  template <typename Aut>
  inline
  auto
  determinize_weight(const Aut& a)
    -> detweighted_automaton<Aut>
  {
    auto res = make_shared_ptr<detweighted_automaton<Aut>>(a);
    // Determinize.
    res->operator()();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      determinize_weight(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::determinize_weight(a));
      }

      REGISTER_DECLARE(determinize_weight,
                       (const automaton& aut) -> automaton);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
