#pragma once

#include <iostream>
#include <queue>
#include <set>
#include <type_traits>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/labelset/stateset.hh>
#include <vcsn/misc/map.hh> // vcsn::has
#include <vcsn/misc/unordered_map.hh> // vcsn::has

namespace vcsn
{
  namespace detail
  {
    template <typename Then, typename Else>
    auto static_if(std::true_type, Then then, Else)
    {
      return then;
    }

    template <typename Then, typename Else>
    auto static_if(std::false_type, Then, Else else_)
    {
      return else_;
    }

    template <Automaton Aut,
              wet_kind_t Kind = detail::wet_kind<labelset_t_of<Aut>,
                                                 weightset_t_of<Aut>>()>
    class polystate_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<Aut, Ctx>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      /// Labels and weights.
      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      /// State index.
      using state_t = state_t_of<automaton_t>;
      using stateset_t = stateset<automaton_t>;

      /// The state name: set of (input) states.
      using state_nameset_t
        = polynomialset<context<stateset_t, weightset_t>, Kind>;
      using state_name_t = typename state_nameset_t::value_t;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      polystate_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
      {
        // Pre.
        auto pre = zero();
        ns_.set_weight(pre, input_->pre(), ws_.one());
        todo_.push(map_.emplace(pre, super_t::pre()).first);

        // Final states.
        for (auto t : final_transitions(input_))
          ns_.set_weight(finals_, input_->src_of(t), input_->weight_of(t));
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      /// The empty polynomial of states.
      state_name_t zero() const
      {
        return static_if
          (bool_constant<Kind == wet_kind_t::bitset>{},
           [] (const auto& self) { return state_name_t(self.state_size_); },
           [] (const auto& self) { return self.ns_.zero(); })
          (*this);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {}, bool delimit = false) const
      {
        auto i = origins().find(s);
        if (i == std::end(origins()))
          this->print_state(s, o);
        else
          {
            if (delimit)
              o << '{';
            ns_.print(i->second, o, fmt, ", ");
            if (delimit)
              o << '}';
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t& origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }

      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state_(const state_name_t& n)
      {
        state_t res;
        auto i = map_.find(n);
        if (i == std::end(map_))
          {
            res = this->new_state();
            todo_.push(map_.emplace(n, res).first);
            auto w = ns_.scalar_product(n, finals_);
            if (!ws_.is_zero(w))
              this->set_final(res, w);
          }
        else
          res = i->second;
        return res;
      }

      /// Map from state name to state number.
      using map_t = std::unordered_map<state_name_t, state_t,
                                       vcsn::hash<state_nameset_t>,
                                       vcsn::equal_to<state_nameset_t>>;
      map_t map_;

      /// Input automaton.
      automaton_t input_;

      /// Its weightset.
      weightset_t ws_ = *input_->weightset();

      /// (Nameset) The polynomialset that stores weighted states.
      state_nameset_t ns_ = {{stateset_t(input_), ws_}};

      /// The sets of (input) states waiting to be processed.
      using queue_t = std::queue<typename map_t::const_iterator>;
      queue_t todo_;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;

      /// Set of final states in the input automaton.
      state_name_t finals_ = zero();
    };

    /// A polystate automaton as a shared pointer.
    template <Automaton Aut,
              wet_kind_t Kind = wet_kind<labelset_t_of<Aut>,
                                         weightset_t_of<Aut>>()>
    using polystate_automaton
      = std::shared_ptr<polystate_automaton_impl<Aut, Kind>>;

    template <Automaton Aut,
              wet_kind_t Kind = wet_kind<labelset_t_of<Aut>,
                                         weightset_t_of<Aut>>()>
    auto
    make_polystate_automaton(const Aut& aut)
      -> polystate_automaton<Aut, Kind>
    {
      using res_t = polystate_automaton<Aut, Kind>;
      return make_shared_ptr<res_t>(aut);
    }
  }
} // namespace vcsn
