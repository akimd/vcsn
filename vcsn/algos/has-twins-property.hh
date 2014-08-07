#ifndef VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
# define VCSN_ALGOS_HAS_TWINS_PROPERTY_HH

# include <stack>
# include <vector>
# include <unordered_set>
# include <unordered_map>

# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/unordered_map.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*----------.
  |  reverse  |
  `----------*/

  /// Inverse the weight of all edges of \a aut.
  template <typename Aut>
  Aut&
  inverse_here(Aut& aut)
  {
    const auto& ws = *aut->weightset();
    for (auto t : aut->all_transitions())
      aut->set_weight(t, ws.rdiv(ws.one(), aut->weight_of(t)));
    return aut;
  }

  template <typename Aut>
  auto
  inverse(const Aut& aut)
    -> decltype(::vcsn::copy(aut))
  {
    auto res = copy(aut);
    return inverse_here(res);
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      automaton
      inverse(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::inverse(a));
      }

      REGISTER_DECLARE(inverse,
                       (const automaton&) -> automaton);
    }
  }


  /*-------------.
  | scc_tarjan.  |
  `-------------*/

  namespace detail
  {
    /// Use Tarjan's algorithm to find all strongly
    /// connected components.
    template <typename Aut>
    class scc_tarjan
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t>;
      using components_t = std::vector<component_t>;

      scc_tarjan(const Aut& aut)
      {
        for (auto s : aut->states())
          {
            if (!has(marked_, s))
              dfs(s, aut);
          }
      }

      const components_t components()
      {
        return components_;
      }

    private:
      void dfs(state_t s, const Aut& aut)
      {
        int min = curr_vertex_num_++;
        low_.emplace(s, min);
        marked_.emplace(s);
        stack_.push(s);

        for (auto t : aut->out(s))
          {
            auto dst = aut->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst, aut);
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
        do
          {
            w = stack_.top();
            stack_.pop();
            components_[curr_comp_num_].emplace(w);
            // This vertex belong only one component
            // so remove it by update low value to max size.
            low_[w] = aut->num_states() + 1;
          }
        while (w != s);
        curr_comp_num_++;
      }

      /// The current component number.
      int curr_comp_num_ = 0;
      /// The current visited vertex
      int curr_vertex_num_ = 0;
      /// All compnents
      components_t components_;
      /// List visited vertices
      std::unordered_set<state_t> marked_;
      /// low_[s] is minimum of vertex that it can go
      std::unordered_map<state_t, int> low_;
      /// Contains list vertices same the component
      std::stack<state_t> stack_;
    };
  }

  /// Find all strongly connected components of \a aut.
  template <typename Aut>
  const std::vector<std::unordered_set<state_t_of<Aut>>>
  components(const Aut& aut)
  {
    detail::scc_tarjan<Aut> scc(aut);
    return scc.components();
  }


  /*--------------------.
  | cycle_unambiguous.  |
  `--------------------*/

  template <typename Aut>
  Aut
  aut_of_component(const std::unordered_set<state_t_of<Aut>>& com,
                   const Aut& aut)
  {
    auto res = make_shared_ptr<Aut>(aut->context());
    std::unordered_map<state_t_of<Aut>, state_t_of<Aut>> map;
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

  /// Whether \a aut is cycle-unambiguous.
  template <typename Aut>
  bool is_cycle_unambiguous(const Aut& aut)
  {
    // Find all strongly connected components
    const auto& coms = components(aut);
    if (getenv("VCSN_DEBUG"))
      {
        std::cerr << "number states of automaton: " <<
          aut->num_states() << std::endl;
        std::cerr << "number components: " <<
          coms.size() << std::endl;
      }

    // Check each component if it is cycle-unambiguous.
    if (coms.size() == 1)
      return is_cycle_unambiguous_scc(aut);
    for (const auto &c : coms)
      {
        const auto& a = aut_of_component(c, aut);
        if (is_cycle_unambiguous_scc(a))
          return true;
      }
    return false;
  }

  /// Whether \a aut is cycle-unambiguous.
  /// Precondition: aut is a strongly connected component.
  template <typename Aut>
  bool is_cycle_unambiguous_scc(const Aut& aut)
  {
    auto prod = product(aut, aut);
    auto coms = components(prod);
    auto origins = prod->origins();
    bool cond_equal, cond_not_equal;
    // In one strong connected component,
    // if exist two states (s1, s1) and (s1, s2)
    // with s1 # s2 then it has two cycle same label
    // from s->s1-> s and s->s2->s.
    for (auto c : coms)
      {
        cond_equal = cond_not_equal = false;
        for (auto s : c)
          {
            auto p = origins[s];
            if (std::get<0>(p) != std::get<1>(p))
              cond_equal = true;
            else
              cond_not_equal = true;
            if (cond_equal && cond_not_equal)
              return true;
          }
      }
    return false;
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      bool
      is_cycle_unambiguous(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::is_cycle_unambiguous(a);
      }

      REGISTER_DECLARE(is_cycle_unambiguous,
                       (const automaton&) -> bool);
    }
  }


  /*-----------------.
  | cycle_identity.  |
  `-----------------*/

  namespace detail
  {
    /// Whether the weight of beetween two states on component,
    /// it is always unique.
    template <typename Aut>
    class cycle_identity_impl
    {
    public:
      using transition_t = transition_t_of<Aut>;
      using weight_t = weight_t_of<Aut>;
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t> ;

      cycle_identity_impl() {}

      // Calcule the weight with depth first search by weight
      // and compare the weight of two state is unique.
      bool check(const component_t& component, const Aut& aut)
      {
        std::unordered_map<state_t, weight_t> wm;
        const auto& ws = *aut->weightset();
        auto s0 = *component.begin();
        std::stack<state_t> todo;
        todo.push(s0);
        wm[s0] = ws.one();
        while (!todo.empty())
          {
            auto s = todo.top();
            todo.pop();
            for (auto t : aut->out(s))
              {
                auto dst = aut->dst_of(t);
                if (has(component, dst))
                  {
                    if (!has(wm, dst))
                      {
                        todo.push(dst);
                        wm.emplace(dst, ws.mul(wm[s], aut->weight_of(t)));
                      }
                    else if (!ws.equals(wm[dst], ws.mul(wm[s], aut->weight_of(t))))
                      return false;
                  }
              }
          }
        return true;
      }
    };
  }

  /// Check the weight of two states on this component is unique.
  template <typename Aut>
  bool cycle_identity(const std::unordered_set<state_t_of<Aut>>& c,
                      const Aut& aut)
  {
    detail::cycle_identity_impl<Aut> ci;
    return ci.check(c, aut);
  }


  /*---------------------.
  | has_twins_property.  |
  `---------------------*/

  /// Whether \a aut has the twins property.
  template <typename Aut>
  bool has_twins_property(const Aut& aut)
  {
    bool is_cycle_uabg = is_cycle_unambiguous(aut);
    assert(!is_cycle_uabg);
    auto trim = ::vcsn::trim(aut);
    auto inv = inverse(trim);
    auto a = ::vcsn::product(inv, trim);

    // Find all components of automate a.
    auto cs = components(a);

    // Check unique weight of two states on each component.
    for (auto c : cs)
      if (!cycle_identity(c, a))
        return false;

    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      bool has_twins_property(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_twins_property(a);
      }

      REGISTER_DECLARE(has_twins_property,
                       (const automaton&) -> bool);
    }
  }
}
#endif // !VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
