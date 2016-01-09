#pragma once

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/fwd.hh> // expression_automaton
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/misc/bimap.hh>
#include <vcsn/misc/symbol.hh>

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

      /// State are named by expressions.
      using state_name_t = expression_t;

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

      /// Storage for state names.
      using left_t
        = boost::bimaps::unordered_set_of<expression_t,
                                          vcsn::hash<expressionset_t>,
                                          vcsn::equal_to<expressionset_t>>;
      /// Storage for state index.
      using right_t = boost::bimaps::set_of<state_t>;
      /// Bijective map state_name_t -> state_t;
      using bimap_t = boost::bimap<left_t, right_t>;

      /// The state for expression \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const expression_t& r)
      {
        state_t res;
        const auto& map_ = bimap_.left;
        auto i = map_.find(r);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            bimap_.insert({r, res});
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
      using origins_t = typename bimap_t::right_map;
      const origins_t& origins() const
      {
        return bimap_.right;
      }

      //    private:
      /// The expression's set.
      expressionset_t rs_;
      /// States to visit.
      std::stack<expression_t, std::vector<expression_t>> todo_;
      /// expression -> state.
      bimap_t bimap_;
    };
  }
}
