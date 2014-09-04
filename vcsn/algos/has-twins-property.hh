#ifndef VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
# define VCSN_ALGOS_HAS_TWINS_PROPERTY_HH

# include <stack>
# include <unordered_set>
# include <unordered_map>
# include <vector>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/scc.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/unordered_map.hh>

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

  /*------------------.
  | cycle_ambiguous.  |
  `------------------*/

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

  /// Whether \a aut is cycle-nambiguous.
  template <typename Aut>
  bool is_cycle_ambiguous(const Aut& aut)
  {
    // Find all strongly connected components.
    const auto& coms = scc(aut);
    if (getenv("VCSN_DEBUG"))
      {
        std::cerr << "number states of automaton: " <<
          aut->num_states() << std::endl;
        std::cerr << "number components: " <<
          coms.size() << std::endl;
      }

    // Check each component if it is cycle-ambiguous.
    if (coms.size() == 1)
      return is_cycle_ambiguous_scc(aut);
    for (const auto &c : coms)
      {
        const auto& a = aut_of_component(c, aut);
        if (is_cycle_ambiguous_scc(a))
          return true;
      }
    return false;
  }

  /// Whether \a aut is cycle-ambiguous.
  /// Precondition: aut is a strongly connected component.
  template <typename Aut>
  bool is_cycle_ambiguous_scc(const Aut& aut)
  {
    auto prod = product(aut, aut);
    auto coms = scc(prod);
    auto origins = prod->origins();
    bool cond_equal, cond_not_equal;
    // In one SCC of prod, if there exist two states (s0, s0) and (s1,
    // s2) with s1 != s2 then aut has two cycles with the same label:
    // s0->s1->s0 and s0->s2->s0.
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
      is_cycle_ambiguous(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::is_cycle_ambiguous(a);
      }

      REGISTER_DECLARE(is_cycle_ambiguous,
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
    require(!is_cycle_ambiguous(aut),
            "has_twins_property: requires a cycle-unambiguous automaton");

    auto trim = ::vcsn::trim(aut);
    auto inv = inverse(trim);
    auto a = product(inv, trim);

    // Find all components of automate a.
    auto cs = scc(a);

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
