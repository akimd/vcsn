#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/misc/zip-maps.hh>

namespace vcsn
{

  /*-----------------.
  | pair_automaton.  |
  `-----------------*/

  namespace detail
  {

    /// The pair automaton is used by several algorithms for
    /// synchronizing words.
    template <typename Aut>
    class pair_automaton_impl
      : public automaton_decorator<mutable_automaton<context_t_of<Aut>>>
    {
    public:
      using automaton_t =  Aut;
      using automaton_nocv_t = mutable_automaton<context_t_of<Aut>>;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using super_t = automaton_decorator<automaton_nocv_t>;

    private:
      /// The semantics of the result states: ordered pair of input
      /// states.
      using pair_t = std::pair<state_t, state_t>;
      using origins_t = std::map<state_t, pair_t>;

    public:
      pair_automaton_impl(const automaton_t& aut, bool keep_initials = false)
        : super_t(aut->context())
        , input_(aut)
        , transition_map_(aut)
        , keep_initials_(keep_initials)
      {
        auto ctx = input_->context();
        auto ws = ctx.weightset();

        if (!keep_initials_)
          {
            q0_ = this->new_state(); // q0 special state
            for (auto l : input_->labelset()->genset())
              this->add_transition(q0_, q0_, l, ws->one());
          }
        else
          for (auto s : input_->states())
            pair_states_.emplace(std::make_pair(s, s), this->new_state());

        // States are "ordered": (s1, s2) is defined only for s1 < s2.
        {
          auto states = input_->states();
          auto end = std::end(states);
          for (auto i1 = std::begin(states); i1 != end; ++i1)
            {
              // FIXME: cannot use i2 = std::next(i1) with clang 3.5
              // and Boost 1.55.
              // https://svn.boost.org/trac/boost/ticket/9984
              auto i2 = i1;
              for (++i2; i2 != end; ++i2)
                // s1 < s2, no need for make_ordered_pair.
                pair_states_.emplace(std::make_pair(*i1, *i2),
                                     this->new_state());
            }
        }

        for (auto ps : pair_states_)
          {
            auto pstates = ps.first; // pair of states
            auto cstate = ps.second; // current state

            for (const auto& p : zip_maps(transition_map_[pstates.first],
                                          transition_map_[pstates.second]))
              this->add_transition(cstate,
                                  state_(std::get<0>(p.second).dst,
                                         std::get<1>(p.second).dst),
                                  p.first, ws->one());
          }

        for (const auto& p: pair_states_)
          origins_.emplace(p.second, p.first);

        if (keep_initials_)
          for (auto s : input_->states())
            singletons_.push_back(state_(s, s));
        else
          singletons_.push_back(q0_);
      }

      state_t get_q0() const
      {
        require(!keep_initials_,
                "can't get_q0() on a pairer that keeps origins");
        return q0_;
      }

      bool is_singleton(state_t s) const
      {
        if (keep_initials_)
          {
            pair_t p = get_origin(s);
            return p.first == p.second;
          }
        else
          return s == q0_;
      }

      const std::vector<state_t>& singletons()
      {
        return singletons_;
      }

      static std::string sname()
      {
        return "pair_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full) const
      {
        return "pair_automaton<" + input_->vname(full) + ">";
      }

      const std::unordered_map<pair_t, state_t>& get_map_pair() const
      {
        return pair_states_;
      }

      /// A map from result state to tuple of original states.
      const origins_t& origins() const
      {
        return origins_;
      }

      const pair_t get_origin(state_t s) const
      {
        auto i = origins().find(s);
        require(i != std::end(origins()), "state not found in origins");
        return i->second;
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
            input_->print_state_name(i->second.first, o, fmt);
            o << ", ";
            input_->print_state_name(i->second.second, o, fmt);
          }
        return o;
      }

    private:
      /// The state in the result automaton that corresponds to (s1,
      /// s2).  Allocate it if needed.
      state_t state_(state_t s1, state_t s2)
      {
        // Benches show it is slightly faster to handle this case
        // especially rather that mapping these "diagonal states" to
        // q0_ in pair_states_.
        if (s1 == s2 && !keep_initials_)
          return q0_;
        else
          return pair_states_[make_ordered_pair(s1, s2)];
      }

      /// Input automaton.
      automaton_t input_;
      /// Fast maps label -> (weight, label).
      using transition_map_t
        = transition_map<automaton_t, weightset_t_of<automaton_t>, true>;
      transition_map_t transition_map_;
      std::unordered_map<pair_t, state_t> pair_states_;
      origins_t origins_;
      std::vector<state_t> singletons_;
      state_t q0_;
      bool keep_initials_ = false;
    };
  }

  template <typename Aut>
  using pair_automaton
    = std::shared_ptr<detail::pair_automaton_impl<Aut>>;

  /*------------------.
  | pair(automaton).  |
  `------------------*/

  template <typename Aut>
  pair_automaton<Aut> pair(const Aut& aut, bool keep_initials = false)
  {
    auto res = make_shared_ptr<pair_automaton<Aut>>(aut, keep_initials);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename>
      automaton
      pair(const automaton& aut, bool keep_initials = false)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::pair(a, keep_initials));
      }

      REGISTER_DECLARE(pair, (const automaton&, bool) -> automaton);
    }
  }
}
