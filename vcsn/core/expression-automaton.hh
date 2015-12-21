#pragma once

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/fwd.hh> // expression_automaton
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/misc/map.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/unordered_map.hh>

//# define DEBUG 1

#if DEBUG
# define DEBUG_IFELSE(Then, Else) Then
#else
# define DEBUG_IFELSE(Then, Else) Else
#endif

#define DEBUG_IF(Then) DEBUG_IFELSE(Then,)

namespace vcsn
{
  namespace detail
  {
    /// An incremental automaton whose states are expressions.
    template <Automaton Aut>
    class expression_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using context_t = context_t_of<automaton_t>;
      using expressionset_t = expressionset<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using state_t = state_t_of<super_t>;
      using label_t = label_t_of<super_t>;
      using weight_t = weight_t_of<super_t>;

      expression_automaton_impl(const expressionset_t& rs)
        : super_t(rs.context())
        , rs_(rs)
      {}

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"expression_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "expression_automaton<";
        super_t::print_set(o, fmt);
        return o << '>';
      }

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<expression_t, state_t,
                                      vcsn::hash<expressionset_t>,
                                      vcsn::equal_to<expressionset_t>>;

      /// The state for expression \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const expression_t& r)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(r);
        if (i == std::end(map_))
          {
            DEBUG_IF(
                     std::cerr << "New state: ";
                     rs_.print(r, std::cerr) << '\n';
                     );
            res = super_t::new_state();
            map_[r] = res;
            todo_.push(r);
          }
        else
          res = i->second;
        return res;
      }

      using super_t::add_transition;
      void
      add_transition(state_t src, const expression_t& dst,
                     label_t l, const weight_t& w)
      {
        super_t::add_transition(src, state(dst), l, w);
      }

      using super_t::new_transition;
      void
      new_transition(state_t src, const expression_t& dst,
                     label_t l, const weight_t& w)
      {
        super_t::new_transition(src, state(dst), l, w);
      }

      using super_t::set_initial;
      void
      set_initial(const expression_t& s, const weight_t& w)
      {
        super_t::set_initial(state(s), w);
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool = false) const
      {
        auto i = origins().find(s);
        if (i == std::end(origins()))
          this->print_state(s, o);
        else
          rs_.print(i->second, o, fmt);
        return o;
      }

      /// Ordered map: state -> its derived term.
      using origins_t = std::map<state_t, expression_t>;
      mutable origins_t origins_;
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      /// The expression's set.
      expressionset_t rs_;
      /// States to visit.
      std::stack<expression_t, std::vector<expression_t>> todo_;
      /// expression -> state.
      smap map_;
    };
  }
}
