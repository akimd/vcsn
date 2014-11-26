#ifndef VCSN_ALGOS_SCC_HH
# define VCSN_ALGOS_SCC_HH

# include <limits>

# include <boost/range/irange.hpp>

# include <vcsn/algos/filter.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/misc/crange.hh>
# include <vcsn/core/partition-automaton.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/builtins.hh>
# include <vcsn/misc/unordered_map.hh>
# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/vector.hh> // has

namespace vcsn
{
  namespace detail
  {

    /*--------------------------------.
    | strongly connected component.   |
    `--------------------------------*/

    /// A strongly-connected component: set of states.
    /// Bench show that using std::unordered_set is better than
    /// std::set about ~10x. For example:
    /// std::set:
    ///   5.53s: a.scc("tarjan_iterative") # a = std((abc)*{1000})
    /// std::unordereset:
    ///   0.58s: a.scc("tarjan_iterative") # a = std((abc)*{1000})
    template <typename Aut>
    using component_t = std::unordered_set<state_t_of<Aut>>;

    /// A set of strongly-connected components.
    template <typename Aut>
    using components_t = std::vector<component_t<Aut>>;



    /*--------------------.
    | tarjan_iterative.   |
    `--------------------*/

    /// Tarjan's algorithm to find all strongly connected components:
    /// iterative implementation.
    ///
    /// Often slightly slower than the recursive implementation, but
    /// no limitation due to the stack.
    template <typename Aut>
    class scc_tarjan_iterative_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using transition_t = transition_t_of<Aut>;

      using component_t = detail::component_t<Aut>;
      using components_t = detail::components_t<Aut>;

      scc_tarjan_iterative_impl(const Aut& aut)
        : aut_{aut}
      {
        for (auto s : aut_->states())
          if (!has(number_, s))
            dfs(s);
      }

      const components_t& components() const
      {
        return components_;
      }

    private:
      void dfs(state_t s)
      {
        number_[s] = low_[s] = curr_vertex_num_++;
        dfs_stack_.emplace_back(s, aut_->out(s).begin(),
                                aut_->out(s).end());
        stack_.push_back(s);
        while (!dfs_stack_.empty())
          {
            auto& st = dfs_stack_.back();
            auto src = st.state;
            if (st.pos != st.end)
              {
                auto dst = aut_->dst_of(*st.pos);
                ++st.pos;
                if (!has(number_, dst))
                  {
                    number_[dst] = low_[dst] = curr_vertex_num_++;
                    const auto& out = aut_->out(dst);
                    dfs_stack_.emplace_back(dst, out.begin(),
                                            out.end());
                    stack_.push_back(dst);
                  }
                else if (low_[dst] < low_[src])
                  low_[src] = low_[dst];
              }
            else
              {
                if (low_[src] == number_[src])
                  {
                    components_.emplace_back(component_t{});
                    auto& com = components_.back();
                    state_t w;
                    do
                      {
                        w = stack_.back();
                        stack_.pop_back();
                        com.emplace(w);
                        low_[w] = low_max_;
                      }
                    while (w != src);
                  }
                dfs_stack_.pop_back();
                if (!dfs_stack_.empty())
                  {
                    auto& parent = dfs_stack_.back();
                    if (low_[src] < low_[parent.state])
                      low_[parent.state] = low_[src];
                  }
              }
          }
      }

      /// Input automaton.
      Aut aut_;

      /// Stack used to simulate with dfs recursive.
      struct step_t;
      std::vector<step_t> dfs_stack_;

      /// The current visited vertex.
      std::size_t curr_vertex_num_ = 0;
      /// Store the visit order of each state.
      std::unordered_map<state_t, std::size_t> number_;
      /// low_[s] is minimum of vertex that it can go.
      std::unordered_map<state_t, std::size_t> low_;
      /// the maximum possible of a value in low_.
      std::size_t low_max_ = std::numeric_limits<unsigned int>::max();

      /// Contains list vertices same the component.
      std::vector<state_t> stack_;
      /// All components.
      components_t components_;

      /// Iterator on outgoing transitions.
      using iterator_t = decltype(aut_->out(state_t{}).begin());

      /// Step of one state contain infomation next successor and end
      /// iterator(output transitions or successors of this state).
      struct step_t
      {
        step_t(state_t s, iterator_t p, iterator_t e)
          : state{s}, pos{p}, end{e} {}
        state_t state;
        iterator_t pos;
        iterator_t end;
      };
    };


    /*--------------------.
    | tarjan_recursive.   |
    `--------------------*/

    /// Tarjan's algorithm to find all strongly connected components:
    /// recursive implementation.
    ///
    /// Often slightly faster than the iterative implementation, but
    /// may overflow the stack.
    template <typename Aut>
    class scc_tarjan_recursive_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = detail::component_t<Aut>;
      using components_t = detail::components_t<Aut>;

      scc_tarjan_recursive_impl(const Aut& aut)
        : aut_{aut}
      {
        for (auto s : aut_->states())
          if (!has(marked_, s))
            dfs(s);
      }

      const components_t& components() const
      {
        return components_;
      }

    private:
      void dfs(state_t s)
      {
        std::size_t min = curr_vertex_num_++;
        low_.emplace(s, min);
        marked_.emplace(s);
        stack_.push_back(s);

        for (auto t : aut_->out(s))
          {
            auto dst = aut_->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst);
            if (low_[dst] < min)
              min = low_[dst];
          }
        if (min < low_[s])
          {
            low_[s] = min;
            return;
          }

        components_.emplace_back(component_t{});
        auto& com = components_.back();
        state_t w;
        do
          {
            w = stack_.back();
            stack_.pop_back();
            com.emplace(w);
            // This vertex belong only one component
            // so remove it by update low value to max size.
            low_[w] = std::numeric_limits<size_t>::max();
          }
        while (w != s);
      }

      /// Input automaton.
      Aut aut_;
      /// The current visited vertex.
      /// It used to preorder number counter.
      std::size_t curr_vertex_num_ = 0;
      /// All components.
      components_t components_;
      /// Visited vertices.
      std::unordered_set<state_t> marked_;
      /// low_[s] is minimum of low_{X},
      /// with X is all states on output transitions of s.
      std::unordered_map<state_t, std::size_t> low_;
      /// Contains list vertices same the component.
      std::vector<state_t> stack_;
    };


    /*---------------------.
    | reverse_postorder.   |
    `---------------------*/

    /// Get all vertices in reverse postorder
    /// by using depth first search.
    template <typename Aut>
    class reverse_postorder_impl
    {
    public:
      using state_t = state_t_of<Aut>;

      reverse_postorder_impl(const Aut& aut)
        : aut_{aut}
      {
        for (auto s : aut->states())
          if (!has(marked_, s))
            dfs(s);
      }

      std::vector<state_t>& reverse_post()
      {
        return rvp_;
      }

    private:
      void dfs(state_t s)
      {
        marked_.emplace(s);
        for (auto t : aut_->out(s))
          {
            auto dst = aut_->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst);
          }
        rvp_.push_back(s);
      }

      /// Input automaton.
      Aut aut_;
      /// Revert postorder of dfs.
      std::vector<state_t> rvp_;
      /// Store the visited states.
      std::set<state_t> marked_;
    };
  }

  /// Get all vertices in reverse postorder.
  template <typename Aut>
  std::vector<state_t_of<Aut>>
  reverse_postorder(const Aut& aut)
  {
    detail::reverse_postorder_impl<Aut> dv(aut);
    return dv.reverse_post();
  }


  /*---------------.
  | scc_kosaraju.  |
  `---------------*/

  namespace detail
  {
    /// Use Kosajaju algorithm for finding all of strongly
    /// connected components.
    template <typename Aut>
    class scc_kosaraju_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = detail::component_t<Aut>;
      using components_t = detail::components_t<Aut>;

      scc_kosaraju_impl(const Aut& aut)
        : aut_{aut}
      {
        auto trans = ::vcsn::transpose(aut);
        auto todo = ::vcsn::reverse_postorder(trans);
        while (!todo.empty())
          {
            auto s = todo.back();
            todo.pop_back();
            if (!has(marked_, s))
              {
                dfs(s);
                ++num_;
              }
          }
      }

      const components_t& components() const
      {
        return components_;
      }

    private:
      void dfs(state_t s)
      {
        marked_.emplace(s);
        if (num_ == components_.size())
          components_.emplace_back(component_t{s});
        else
          components_[num_].emplace(s);

        for (auto t : aut_->out(s))
          {
            auto dst = aut_->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst);
          }
      }

      /// Input automaton.
      Aut aut_;
      /// The current component number.
      std::size_t num_ = 0;
      // All components.
      components_t components_;
      std::unordered_set<state_t> marked_;
    };
  }

  enum class scc_algo_t
  {
    tarjan_iterative,
    tarjan_recursive,
    kosaraju
  };

  inline scc_algo_t scc_algo(const std::string& algo)
  {
    scc_algo_t res;
    if (algo == "auto" || algo == "tarjan_iterative")
      res = scc_algo_t::tarjan_iterative;
    else if (algo == "tarjan_recursive")
      res = scc_algo_t::tarjan_recursive;
    else if (algo == "kosaraju")
      res = scc_algo_t::kosaraju;
    else
      raise("sccs: invalid algorithm: ", str_escape(algo));
    return res;
  }

  /// Find all strongly connected components of \a aut.
  template <typename Aut>
  const detail::components_t<Aut>
  strong_components(const Aut& aut,
                    scc_algo_t algo = scc_algo_t::tarjan_iterative)
  {
    switch (algo)
      {
      case scc_algo_t::tarjan_recursive:
        {
          detail::scc_tarjan_recursive_impl<Aut> scc{aut};
          return scc.components();
        }
      case scc_algo_t::tarjan_iterative:
        {
          detail::scc_tarjan_iterative_impl<Aut> scc{aut};
          return scc.components();
        }
      case scc_algo_t::kosaraju:
        {
          detail::scc_kosaraju_impl<Aut> scc{aut};
          return scc.components();
        }
      }
    BUILTIN_UNREACHABLE();
  }

  /// Generate a subautomaton corresponding to an SCC.
  template <typename Aut>
  typename Aut::element_type::automaton_nocv_t
  aut_of_component(const detail::component_t<Aut>& com, const Aut& aut)
  {
    using res_t = typename Aut::element_type::automaton_nocv_t;
    res_t res = make_shared_ptr<res_t>(aut->context());
    std::unordered_map<state_t_of<Aut>, state_t_of<res_t>> map;
    auto s0 = *com.begin();
    map[s0] = res->new_state();
    res->set_initial(map[s0]);
    for (auto s : com)
      {
        if (!has(map, s))
          map[s] = res->new_state();
        for (auto t : aut->out(s))
          {
            auto dst = aut->dst_of(t);
            if (!has(com, dst))
              continue;
            if (!has(map, dst))
              map[dst] = res->new_state();
            res->new_transition(map[s], map[dst], aut->label_of(t));
          }
      }
    return res;
  }

  /*-----------------.
  | scc_automaton.   |
  `-----------------*/

  namespace detail
  {
    /// An automaton decorator that maps states to their scc-number.
    template <typename Aut>
    class scc_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<Aut>;
      using components_t = detail::components_t<Aut>;
      using component_t = detail::component_t<Aut>;

      scc_automaton_impl(const automaton_t& input, scc_algo_t algo)
        : super_t(input)
      {
        components_ = ::vcsn::strong_components(aut_, algo);
        size_t num_sccs = components_.size();
        for (size_t i = 0; i < num_sccs; ++i)
          for (auto s : components_[i])
            component_[s] = i + 1;
      }

      /// Static name.
      static symbol sname()
      {
        static symbol res("scc_automaton<"
                          + automaton_t::element_type::sname() + '>');
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "scc_automaton<";
        aut_->print_set(o, format);
        return o << '>';
      }

      bool state_has_name(state_t s) const
      {
        return s != super_t::pre() && s != super_t::post();
      }

      std::ostream& print_state_name(state_t s, std::ostream& o,
                                     const std::string& format = "text",
                                     bool = false) const
      {
        o << component_.at(s) << ".";
        aut_->print_state_name(s, o, format, true);
        return o;
      }

      const component_t component(unsigned com_num) const
      {
        if (com_num < 0 || components_.size() <= com_num)
          raise("component: com_num must have between 0 and ",
                components_.size());
        return components_[com_num];
      }

      const components_t components() const
      {
        return components_;
      }

      size_t num_components() const
      {
        return components_.size();
      }
    private:
      using super_t::aut_;
      /// Store the component number of a state.
      std::map<state_t, size_t> component_;
      components_t components_;
    };
  }

  template <typename Aut>
  using scc_automaton = std::shared_ptr<detail::scc_automaton_impl<Aut>>;

  /// Get scc_automaton from \a aut.
  template <typename Aut>
  inline
  scc_automaton<Aut>
  scc(const Aut& aut, const std::string& algo)
  {
    return make_shared_ptr<scc_automaton<Aut>>(aut, scc_algo(algo));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      automaton scc(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::scc(a, algo));
      }

      REGISTER_DECLARE(scc,
                       (const automaton&, const std::string& algo)
                       -> automaton);
    }
  }

  /*----------------.
  | num_components. |
  `----------------*/

  /// Get number of strongly connected components.
  template <typename Aut>
  std::size_t num_components(const scc_automaton<Aut>& aut)
  {
    return aut->num_components();
  }

  template <typename Aut>
  std::size_t num_components(const Aut&)
  {
    raise("num_components: requires an scc_automaton");
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      std::size_t num_components(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::num_components(a);
      }

      REGISTER_DECLARE(num_components,
                       (const automaton&) -> std::size_t);
    }
  }


  /*-----------.
  | component. |
  `-----------*/

  /// Get a sub automaton who is a strongly connected component.
  template <typename Aut>
  inline
  filter_automaton<scc_automaton<Aut>>
  component(const scc_automaton<Aut>& aut, unsigned com_num)
  {
    std::unordered_set<state_t_of<Aut>> ss;
    for (auto s : aut->component(com_num - 1))
      ss.emplace(s);
    return vcsn::filter(aut, ss);
  }

  template <typename Aut>
  filter_automaton<scc_automaton<Aut>>
  component(const Aut&, unsigned)
  {
    raise("component: requires an scc_automaton");
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Unsigned>
      automaton component(const automaton& aut, unsigned com_num)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::component(a, com_num));
      }

      REGISTER_DECLARE(component,
                       (const automaton&, unsigned) -> automaton);
    }
  }


  /*----------.
  | condense. |
  `----------*/

  /// Create a condensation of automaton with each its state who is a strongly
  /// connected component of \a aut.
  template <typename Aut>
  partition_automaton<scc_automaton<Aut>>
  condense(const scc_automaton<Aut>& aut)
  {
    auto res = make_shared_ptr<partition_automaton<scc_automaton<Aut>>>(aut);
    using state_t = state_t_of<Aut>;
    /// map from state of aut to state(component) of new automaton.
    std::unordered_map<state_t, state_t> map;

    // Add states to new automaton.
    std::set<state_t> ss;
    for (const auto& com : aut->components())
      {
        ss.clear();
        ss.insert(begin(com), end(com));
        state_t new_state = res->new_state(ss);
        for (auto s : com)
          map[s] = new_state;
      }
    map[aut->pre()] = res->pre();
    map[aut->post()] = res->post();

    // Add transitions to new automaton.
    for (auto t: aut->all_transitions())
      {
        auto s = map[aut->src_of(t)];
        auto d = map[aut->dst_of(t)];
        if (s != d)
          res->add_transition(s, d, aut->label_of(t), aut->weight_of(t));
      }
    return res;
  }

  template <typename Aut>
  partition_automaton<Aut>
  condense(const Aut&)
  {
    raise("condense: requires an scc_automaton");
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton condense(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::condense(a));
      }

      REGISTER_DECLARE(condense,
                       (const automaton&) -> automaton);
    }
  }

}
#endif // !VCSN_ALGOS_SCC_HH
