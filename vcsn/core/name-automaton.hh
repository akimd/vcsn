#pragma once

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <vcsn/core/fwd.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/misc/format.hh>
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
    /// An incremental automaton whose states are names.
    template <typename Aut>
    class name_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using context_t = context_t_of<automaton_t>;
      using state_name_t = symbol;
      using state_t = state_t_of<super_t>;
      using label_t = label_t_of<super_t>;
      using weight_t = weight_t_of<super_t>;

      name_automaton_impl(const context_t& ctx)
        : super_t(ctx)
      {}

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"name_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "name_automaton<";
        super_t::print_set(o, fmt);
        return o << '>';
      }

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<state_name_t, state_t>;

      /// The state for name \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_name_t& r, state_t s)
      {
        map_[r] = s;
        return s;
      }

      /// The state for name \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_name_t& r)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(r);
        if (i == std::end(map_))
          res = state(r, super_t::new_state());
        else
          res = i->second;
        return res;
      }

      using super_t::add_transition;
      void
      add_transition(state_t src, state_name_t dst,
                     label_t l, const weight_t& w)
      {
        super_t::add_transition(src, state(dst), l, w);
      }

      using super_t::new_transition;
      void
      new_transition(state_t src, state_name_t dst,
                     label_t l, const weight_t& w)
      {
        super_t::new_transition(src, state(dst), l, w);
      }

      using super_t::set_initial;
      void
      set_initial(state_name_t s, const weight_t& w)
      {
        super_t::set_initial(state(s), w);
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format = {},
                       bool = false) const
      {
        auto i = origins().find(s);
        if (i == std::end(origins()))
          this->print_state(s, o);
        else
          o << i->second;
        return o;
      }

      /// Ordered map: state -> its name.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      /// state name -> state number.
      smap map_;
    };
  }
}
