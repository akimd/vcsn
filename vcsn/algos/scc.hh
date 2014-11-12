#ifndef VCSN_ALGOS_SCC_HH
# define VCSN_ALGOS_SCC_HH

# include <limits>
# include <stack>

# include <boost/range/irange.hpp>

# include <vcsn/algos/transpose.hh>
# include <vcsn/core/crange.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/builtins.hh>
# include <vcsn/misc/unordered_map.hh>
# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/vector.hh> // has

namespace vcsn
{
  /*-------------------------------.
  | strongly connected component.  |
  `-------------------------------*/

  namespace detail
  {
    /// A strongly-connected component: set of states.
    /// Bench show that using std::unordered_set is better than
    /// std::set about ~10x. For example:
    /// std::set:
    ///   5.53s: a.num_sccs("tarjan_iterative") # a = std((abc)*{1000})
    /// std::unordereset:
    ///   0.58s: a.num_sccs("tarjan_iterative") # a = std((abc)*{1000})
    template <typename Aut>
    using component_t = std::unordered_set<state_t_of<Aut>>;

    /// A set of strongly-connected components.
    template <typename Aut>
    using components_t = std::vector<component_t<Aut>>;


    /// Use Tarjan's algorithm to find all strongly
    /// connected components.
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
        stack_.push(s);

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

        state_t w;
        components_.emplace_back(component_t{});
        auto& com = components_.back();
        do
          {
            w = stack_.top();
            stack_.pop();
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
      /// All compnents.
      components_t components_;
      /// Visited vertices.
      std::unordered_set<state_t> marked_;
      /// low_[s] is minimum of low_{X},
      /// with X is all states on output transitions of s.
      std::unordered_map<state_t, std::size_t> low_;
      /// Contains list vertices same the component.
      std::stack<state_t> stack_;
    };

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
      using tr_cont_t = std::vector<transition_t>;
      using tr_filter_t = container_filter_range<const tr_cont_t&>;
      using iterator_t = typename tr_filter_t::const_iterator;
      struct step_t;

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
      /// All compnents.
      components_t components_;

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
  }


  /*--------------------.
  | reverse_postorder.  |
  `--------------------*/

  namespace detail
  {
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

      std::stack<state_t>& reverse_post()
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
        rvp_.push(s);
      }

      /// Input automaton.
      Aut aut_;
      /// Revert postorder of dfs.
      std::stack<state_t> rvp_;
      /// Store the visited states.
      std::set<state_t> marked_;
    };
  }

  /// Get all vertices in reverse postorder.
  template <typename Aut>
  std::stack<state_t_of<Aut>>
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
            auto s = todo.top();
            todo.pop();
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

  inline scc_algo_t scc_algo_to_enum(const std::string& algo)
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


  /*------------.
  | num_sccs.   |
  `------------*/

  /// Get number of strongly connected components.
  template <typename Aut>
  std::size_t num_sccs(const Aut& aut, const std::string& algo = "auto")
  {
    return strong_components(aut, scc_algo_to_enum(algo)).size();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      std::size_t num_sccs(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::num_sccs(a, algo);
      }

      REGISTER_DECLARE(num_sccs,
                       (const automaton&, const std::string& algo)
                       -> std::size_t);
    }
  }


  /*-----------------.
  | scc_automaton.   |
  `-----------------*/

  namespace detail
  {
    template <typename Aut>
    class scc_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
    public:
      using automaton_t = Aut;
      using automaton_nocv_t =
        typename automaton_t::element_type::automaton_nocv_t;
      using super_t = automaton_decorator<automaton_nocv_t>;
      using state_t = state_t_of<Aut>;

      scc_automaton_impl(const Aut& input, scc_algo_t algo)
        : super_t(input), algo_{algo}
      {}

      void operator()()
      {
        const auto& sccs = ::vcsn::strong_components(aut_, algo_);
        size_t num_sccs = sccs.size();
        for (int i = 0; i < num_sccs; ++i)
          for (auto s : sccs[i])
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
                                     const std::string& fmt = "text",
                                     bool = false) const
      {
        o << component_.at(s) << ".";
        aut_->print_state_name(s, o, fmt, true);
        return o;
      }

    protected:
      using super_t::aut_;

    private:
      /// Store the component number of a state.
      std::map<state_t, size_t> component_;
      std::map<state_t, state_t> map_;

      /// Algorithm apply to find strongly connected component.
      scc_algo_t algo_;
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
    auto res = make_shared_ptr<scc_automaton<Aut>>(aut,
                                                   scc_algo_to_enum(algo));
    res->operator()();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
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

}
#endif // !VCSN_ALGOS_SCC_HH
