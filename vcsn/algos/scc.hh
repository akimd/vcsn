#pragma once

#include <iterator> // std::rbegin
#include <limits>
#include <stack>

#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>

#include <vcsn/algos/copy.hh> // make_fresh_automaton
#include <vcsn/algos/filter.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/partition-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/unordered_map.hh>
#include <vcsn/misc/unordered_set.hh>
#include <vcsn/misc/vector.hh> // has

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


    /*---------------------.
    | reverse_postorder.   |
    `---------------------*/

    /// Get all states in reverse postorder using depth first search.
    template <typename Aut>
    class reverse_postorder_impl
    {
    public:
      using state_t = state_t_of<Aut>;

      reverse_postorder_impl(const Aut& aut)
        : aut_{aut}
      {
        rvp_.reserve(aut->num_all_states());
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
        for (auto t : out(aut_, s))
          {
            auto dst = aut_->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst);
          }
        rvp_.emplace_back(s);
      }

      /// Input automaton.
      Aut aut_;
      /// Revert postorder of dfs.
      std::vector<state_t> rvp_;
      /// Store the visited states.
      std::set<state_t> marked_;
    };
  }

  /// Get all states in reverse postorder.
  template <typename Aut>
  inline
  std::vector<state_t_of<Aut>>
  reverse_postorder(const Aut& aut)
  {
    detail::reverse_postorder_impl<Aut> dv(aut);
    return dv.reverse_post();
  }


  namespace detail
  {
    /*----------------.
    | scc_dijkstra.   |
    `----------------*/

    /// Compute the strongly connected components using Dijkstra's
    /// algorithm.
    ///
    /// https://en.wikipedia.org/wiki/Path-based_strong_component_algorithm
    template <typename Aut>
    class scc_dijkstra_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = detail::component_t<Aut>;
      using components_t = detail::components_t<Aut>;

      scc_dijkstra_impl(const Aut& aut)
        : aut_{aut}
      {}

      const components_t& components()
      {
        for (auto s : aut_->states())
          if (!has(component_, s))
            dfs(s);
        return components_;
      }

    private:
      void dfs(state_t s)
      {
        preorder_[s] = count_;
        ++count_;
        unassigned_.push(s);
        uncertain_.push(s);
        for (auto t: out(aut_, s))
          {
            state_t dst = aut_->dst_of(t);
            if (!has(preorder_, dst))
              dfs(dst);
            else if (!has(component_, dst))
              {
                size_t dstpo = preorder_[dst];
                while (dstpo < preorder_[uncertain_.top()])
                  uncertain_.pop();
              }
          }
        if (s == uncertain_.top())
          {
            uncertain_.pop();
            auto scc_num = components_.size();
            components_.emplace_back();
            component_t& scc = components_.back();
            for (state_t r = unassigned_.top();
                 !unassigned_.empty();
                 unassigned_.pop(), r = unassigned_.top())
              {
                component_[r] = scc_num;
                scc.insert(r);
                if (r == s)
                  break;
              }
          }
      }

      /// Input automaton.
      Aut aut_;
      /// Stack S contains all the vertices that have not yet been
      /// assigned to a strongly connected component, in the order in
      /// which the depth-first search reaches the vertices.
      std::stack<state_t> unassigned_;
      /// Stack P contains vertices that have not yet been determined
      /// to belong to different strongly connected components from
      /// each other.
      std::stack<state_t> uncertain_;
      /// Current state number (not it's id, but its count).
      std::size_t count_ = 0;

      /// For each state, its component number.
      std::unordered_map<state_t, size_t> component_;

      std::unordered_map<state_t, size_t> preorder_;
      /// All the components.
      components_t components_;
    };

    /*----------------.
    | scc_kosaraju.   |
    `----------------*/

    /// Compute the strongly connected components using Kosaraju's
    /// algorithm.
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

        for (auto t : out(aut_, s))
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
      /// All components.
      components_t components_;
      std::unordered_set<state_t> marked_;
    };


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
        number_[s] = low_[s] = curr_state_num_++;
        dfs_stack_.emplace_back(s, out(aut_, s).begin(), out(aut_, s).end());
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
                    number_[dst] = low_[dst] = curr_state_num_++;
                    const auto& ts = out(aut_, dst);
                    dfs_stack_.emplace_back(dst, ts.begin(), ts.end());
                    stack_.push_back(dst);
                  }
                else if (low_[dst] < low_[src])
                  low_[src] = low_[dst];
              }
            else
              {
                if (low_[src] == number_[src])
                  {
                    components_.emplace_back();
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

      /// The current visited state.
      std::size_t curr_state_num_ = 0;
      /// Store the visit order of each state.
      std::unordered_map<state_t, std::size_t> number_;
      /// low_[s] is minimum of state that it can go.
      std::unordered_map<state_t, std::size_t> low_;
      /// the maximum possible of a value in low_.
      std::size_t low_max_ = std::numeric_limits<unsigned int>::max();

      /// List of states in the same the component.
      std::vector<state_t> stack_;
      /// All components.
      components_t components_;

      /// Iterator on outgoing transitions.
      using iterator_t = decltype(out(aut_, state_t{}).begin());

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
        std::size_t min = curr_state_num_++;
        low_.emplace(s, min);
        marked_.emplace(s);
        stack_.push_back(s);

        for (auto t : out(aut_, s))
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

        components_.emplace_back();
        auto& com = components_.back();
        state_t w;
        do
          {
            w = stack_.back();
            stack_.pop_back();
            com.emplace(w);
            // This state belong only one component
            // so remove it by update low value to max size.
            low_[w] = std::numeric_limits<size_t>::max();
          }
        while (w != s);
      }

      /// Input automaton.
      Aut aut_;
      /// The current visited state.
      /// It used to preorder number counter.
      std::size_t curr_state_num_ = 0;
      /// All components.
      components_t components_;
      /// Visited states.
      std::unordered_set<state_t> marked_;
      /// low_[s] is minimum of low_{X},
      /// with X is all states on output transitions of s.
      std::unordered_map<state_t, std::size_t> low_;
      /// List of states in the same the component.
      std::vector<state_t> stack_;
    };
  }


  /*-------.
  | scc.   |
  `-------*/

  enum class scc_algo_t
  {
    dijkstra,
    tarjan_iterative,
    tarjan_recursive,
    kosaraju
  };

  inline scc_algo_t scc_algo(const std::string& algo)
  {
    scc_algo_t res;
    if (algo == "dijkstra")
      res = scc_algo_t::dijkstra;
    else if (algo == "auto" || algo == "tarjan_iterative")
      res = scc_algo_t::tarjan_iterative;
    else if (algo == "tarjan_recursive")
      res = scc_algo_t::tarjan_recursive;
    else if (algo == "kosaraju")
      res = scc_algo_t::kosaraju;
    else
      raise("scc: invalid algorithm: ", str_escape(algo));
    return res;
  }

  /// Find all strongly connected components of \a aut.
  template <typename Aut>
  inline
  const detail::components_t<Aut>
  strong_components(const Aut& aut,
                    scc_algo_t algo = scc_algo_t::tarjan_iterative)
  {
    switch (algo)
      {
      case scc_algo_t::dijkstra:
        return detail::scc_dijkstra_impl<Aut>{aut}.components();
      case scc_algo_t::kosaraju:
        return detail::scc_kosaraju_impl<Aut>{aut}.components();
      case scc_algo_t::tarjan_recursive:
        return detail::scc_tarjan_recursive_impl<Aut>{aut}.components();
      case scc_algo_t::tarjan_iterative:
        return detail::scc_tarjan_iterative_impl<Aut>{aut}.components();
      }
    BUILTIN_UNREACHABLE();
  }

  /// Generate a subautomaton corresponding to an SCC.
  template <typename Aut>
  inline
  fresh_automaton_t_of<Aut>
  aut_of_component(const detail::component_t<Aut>& com, const Aut& aut)
  {
    auto res = make_fresh_automaton(aut);
    using res_t = decltype(res);
    std::unordered_map<state_t_of<Aut>, state_t_of<res_t>> map;
    auto s0 = *com.begin();
    map[s0] = res->new_state();
    res->set_initial(map[s0]);
    for (auto s : com)
      {
        if (!has(map, s))
          map[s] = res->new_state();
        for (auto t : out(aut, s))
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
        // Components are numbered by ordered of discovery.  Reverse
        // this, so that components are roughly numbered as the
        // condensation state numbers would be (0 as an initial
        // state).
        const auto& cs = vcsn::strong_components(aut_, algo);
        // FIXME: std::rbegin/rend does not work with libstdc++.
        components_.assign(boost::rbegin(cs), boost::rend(cs));
        size_t num_sccs = components_.size();
        for (size_t i = 0; i < num_sccs; ++i)
          for (auto s : components_[i])
            component_[s] = i;
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"scc_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "scc_automaton<";
        aut_->print_set(o, fmt);
        return o << '>';
      }

      bool state_has_name(state_t) const
      {
        return true;
      }

      std::ostream& print_state_name(state_t s, std::ostream& o,
                                     format fmt = {},
                                     bool = false) const
      {
        o << component_.at(s) << '.';
        aut_->print_state_name(s, o, fmt, true);
        return o;
      }

      const component_t& component(unsigned num) const
      {
        require(num < components_.size(),
                "component: invalid component number: ",
                num, ": there are ", components_.size(), " components");
        return components_[num];
      }

      const components_t& components() const
      {
        return components_;
      }

      size_t num_components() const
      {
        return components_.size();
      }

    private:
      using super_t::aut_;
      /// For each state, its component number.
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
  scc(const Aut& aut, const std::string& algo = "auto")
  {
    return make_shared_ptr<scc_automaton<Aut>>(aut, scc_algo(algo));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      inline
      automaton scc(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::scc(a, algo));
      }
    }
  }

  /*----------------.
  | num_components. |
  `----------------*/

  /// Get number of strongly connected components.
  template <typename Aut>
  inline
  std::size_t num_components(const scc_automaton<Aut>& aut)
  {
    return aut->num_components();
  }

  template <typename Aut>
  inline
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
      inline
      std::size_t num_components(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::num_components(a);
      }
    }
  }


  /*-----------.
  | component. |
  `-----------*/

  /// An SCC as a subautomaton.
  /// \param aut  the input automaton.
  /// \param num  the number of the scc.
  template <typename Aut>
  inline
  filter_automaton<scc_automaton<Aut>>
  component(const scc_automaton<Aut>& aut, unsigned num)
  {
    return vcsn::filter(aut, aut->component(num));
  }

  template <typename Aut>
  inline
  void
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
      inline
      automaton component(const automaton& aut, unsigned num)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::component(a, num));
      }
    }
  }


  /*----------.
  | condense. |
  `----------*/

  /// Create a condensation of automaton with each its state who is a strongly
  /// connected component of \a aut.
  template <typename Aut>
  inline
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
  inline
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
      inline
      automaton condense(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::condense(a));
      }
    }
  }
}
