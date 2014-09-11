#ifndef VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
# define VCSN_ALGOS_HAS_TWINS_PROPERTY_HH

# include <stack>
# include <unordered_set>
# include <unordered_map>
# include <vector>

# include <vcsn/algos/accessible.hh> // vcsn::trim
# include <vcsn/algos/is-ambiguous.hh> // is_cycle_ambiguous
# include <vcsn/algos/product.hh>
# include <vcsn/algos/scc.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/unordered_set.hh> // vcsn::has
# include <vcsn/misc/unordered_map.hh> // vcsn::has

namespace vcsn
{

  /*---------.
  | invert.  |
  `---------*/

  /// Invert the weight of each transition of \a aut.
  template <typename Aut>
  Aut&
  invert_here(Aut& aut)
  {
    const auto& ws = *aut->weightset();
    for (auto t : aut->all_transitions())
      {
        auto w = aut->weight_of(t);
        aut->set_weight(t, ws.rdiv(ws.one(), w));
      }
    return aut;
  }

  template <typename Aut>
  auto
  invert(const Aut& aut)
    -> decltype(::vcsn::copy(aut))
  {
    auto res = copy(aut);
    return invert_here(res);
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      automaton
      invert(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::invert(a));
      }

      REGISTER_DECLARE(invert,
                       (const automaton&) -> automaton);
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

      /// Compute the weight with depth first search by weight and
      /// compare the weight of two state is unique.
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
                    else if (!ws.equals(wm[dst],
                                        ws.mul(wm[s], aut->weight_of(t))))
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
    auto inv = invert(trim);
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
