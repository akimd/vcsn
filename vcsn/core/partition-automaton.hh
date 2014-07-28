#ifndef VCSN_CORE_PARTITION_AUTOMATON_HH
# define VCSN_CORE_PARTITION_AUTOMATON_HH

# include <map>
# include <set>
# include <vector>

# include <vcsn/core/automaton-decorator.hh>

namespace vcsn
{
  namespace detail
  {

    /// An automaton wrapper whose states form a partition of the
    /// state set of another automaton.
    ///
    /// \tparam Aut the type of the wrapped input automaton.
    template <typename Aut>
    class partition_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
    public:
      using automaton_t = Aut;
      using automaton_nocv_t = typename automaton_t::element_type::automaton_nocv_t;
      using context_t = context_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using super_t = automaton_decorator<automaton_nocv_t>;

      // The underlying state type.  FIXME: isn't this always
      // state_t_of<mutable_automaton>?  I think we need a guarantee of
      // it always being an integer type.  If this is correct then I
      // think state_t should be a separate global definition: something
      // like vcsn::state_t.
      using state_t = state_t_of<automaton_t>;

      /// The state names: a set of the original automaton states.
      using state_name_t = std::set<state_t>;

    private:
      /// A map from each state to the origin state set it stands for.
      using origins_t = std::map<state_t, state_name_t>;
      origins_t origins_;

      /// The input automaton.
      const automaton_t input_;

    public:
      partition_automaton_impl(const automaton_t& input)
        : super_t(input->context())
        , input_(input)
      {}

      // FIXME: do we want this, or should we leave it to the
      // underlying automaton, as super_t does?  Or is this always
      // subclassed anyway?
      static std::string sname()
      {
        return "partition_automaton<" + automaton_t::element_type::sname() + ">";
      }

      /// Dynamic name.
      std::string vname(bool full = true) const
      {
        return "partition_automaton<" + input_->vname(full) + ">";
      }

      bool state_has_name(state_t s) const
      {
        // FIXME: why in the world do I have to write super_t::pre()
        // or this->pre() rather than simply pre()?  Inheritance from
        // super_t is public, and pre is a public method.  I must be
        // missing some fine semantic difference.
        return s != super_t::pre() && s != super_t::post();
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& fmt = "text") const
      {
        const auto& set = origins_.at(s);
        const char* separator = "{";
        for (auto s : set)
          {
            o << separator;
            input_->print_state_name(s, o, fmt);
            separator = ", ";
          }
        return o << "}";
      }

      using super_t::new_state;

      /// Make a new state representing the given input state set,
      /// which is required to be new -- no error-checking on this.
      /// FIXME: shall I keep a reverse-origin for that?
      state_t
      new_state(const state_name_t& set)
      {
        state_t res = new_state();
        origins_[res] = set;
        return res;
      }

      state_t
      new_state(const std::vector<state_t>& v)
      {
        state_name_t set;
        for (auto s: v)
          set.emplace(s);
        return new_state(std::move(set));
      }
    }; // class
  } // namespace detail

  /// A partition automaton as a shared pointer.
  template <typename Aut>
  using partition_automaton
    = std::shared_ptr<detail::partition_automaton_impl<Aut>>;

} // namespace vcsn

#endif // !VCSN_CORE_PARTITION_AUTOMATON_HH
