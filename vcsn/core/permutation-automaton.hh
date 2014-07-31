#ifndef VCSN_CORE_PERMUTATION_DECORATOR_HH
# define VCSN_CORE_PERMUTATION_DECORATOR_HH

# include <map>
# include <queue>
# include <unordered_map>

# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/fwd.hh> // permutation_automaton

namespace vcsn
{
  namespace detail
  {
    /// An automaton isomorphic to another one.
    ///
    /// \tparam Aut the type of the wrapped input automaton.
    template <typename Aut>
    class permutation_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
    public:
      /// Input automaton type.
      using automaton_t = Aut;
      /// Sorted automaton type.
      using automaton_nocv_t = typename automaton_t::element_type::automaton_nocv_t;
      using super_t = automaton_decorator<automaton_nocv_t>;

      /// Symbolic state name: input automaton state type.
      using state_name_t = state_t_of<automaton_t>;
      /// Sorted automaton state type.
      using state_t = state_t_of<automaton_nocv_t>;

    public:
      permutation_automaton_impl(const automaton_t& input)
        : super_t(make_shared_ptr<automaton_nocv_t>(real_context(input)))
        , input_(input)
      {
        map_[input_->pre()] = super_t::pre();
        map_[input_->post()] = super_t::post();
        todo_.push({input_->pre(), super_t::pre()});
      }

      /// Static name.
      static std::string sname()
      {
        return "permutation_automaton<" + automaton_t::element_type::sname() + ">";
      }

      /// Dynamic name.
      std::string vname(bool full = true) const
      {
        return "permutation_automaton<" + input_->vname(full) + ">";
      }

      bool state_has_name(state_t s) const
      {
        return s != super_t::pre() && s != super_t::post();
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& fmt = "text") const
      {
        return input_->print_state_name(origins().at(s), o, fmt);
      }

      state_t
      state(state_name_t s)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(s);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            map_[s] = res;
            todo_.push({s, res});
          }
        else
          res = i->second;
        return res;
      }

      /// A map from each state to the origin state set it stands for.
      using origins_t = std::map<state_t, state_name_t>;

      /// Ordered map: state -> its derived term.
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      using pair_t = std::pair<state_name_t, state_t>;
      std::queue<pair_t> todo_;

      /// Input-state -> sorted-state.
      std::unordered_map<state_name_t, state_t> map_;

      mutable origins_t origins_;

      /// Input automaton.
      const automaton_t input_;
    }; // class
  } // namespace detail
} // namespace vcsn

#endif // ! VCSN_CORE_PERMUTATION_DECORATOR_HH
