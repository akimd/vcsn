#ifndef VCSN_CORE_RATEXP_AUTOMATON_HH
# define VCSN_CORE_RATEXP_AUTOMATON_HH

# include <memory>
# include <stack>
# include <string>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/map.hh>
# include <vcsn/misc/unordered_map.hh>

# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/rat/ratexpset.hh>

//# define DEBUG 1

# if DEBUG
#  define DEBUG_IFELSE(Then, Else) Then
# else
#  define DEBUG_IFELSE(Then, Else) Else
# endif

# define DEBUG_IF(Then) DEBUG_IFELSE(Then,)

namespace vcsn
{
  /*-------------------.
  | ratexp_automaton.  |
  `-------------------*/

  namespace detail
  {
    /// An incremental automaton whose states are ratexps.
    template <typename Aut>
    struct ratexp_automaton_impl : automaton_decorator<Aut>
    {
      using automaton_t = Aut;
      using context_t = context_t_of<Aut>;
      using ratexpset_t = ratexpset<context_t>;
      using ratexp_t = typename ratexpset_t::value_t;
      using super_t = automaton_decorator<mutable_automaton<context_t>>;
      using state_t = state_t_of<super_t>;
      using label_t = label_t_of<super_t>;
      using weight_t = weight_t_of<super_t>;

      ratexp_automaton_impl(const context_t& ctx)
        : super_t(ctx)
        , rs_(ctx)
      {}

      static std::string sname()
      {
        return "ratexp_automaton<" + super_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "ratexp_automaton<" + super_t::vname(full) + ">";
      }

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<ratexp_t, state_t,
                                      vcsn::hash<ratexpset_t>,
                                      vcsn::equal_to<ratexpset_t>>;

      /// The state for ratexp \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const ratexp_t& r)
      {
        state_t dst;
        auto i = map_.find(r);
        if (i == end(map_))
          {
            DEBUG_IF(
                     std::cerr << "New state: ";
                     rs_.print(r, std::cerr) << '\n';
                     );
            dst = super_t::new_state();
            map_[r] = dst;
            todo_.push(r);
          }
        else
          dst = i->second;
        return dst;
      }

      using super_t::add_transition;
      void
      add_transition(const ratexp_t& src, const ratexp_t& dst,
                     const label_t& l, const weight_t& w)
      {
        super_t::add_transition(state(src), state(dst), l, w);
      }

      void
      add_transition(state_t src, const ratexp_t& dst,
                     const label_t& l, const weight_t& w)
      {
        super_t::add_transition(src, state(dst), l, w);
      }

      using super_t::set_initial;
      void
      set_initial(const ratexp_t& s, const weight_t& w)
      {
        super_t::set_initial(state(s), w);
      }

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(std::ostream& o, state_t s,
                       const std::string& fmt) const
      {
        return o << str_escape(format(rs_, origins().at(s), fmt));
      }

      /// Ordered map: state -> its derived term.
      using origins_t = std::map<state_t, ratexp_t>;
      mutable origins_t origins_;
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      /// The ratexp's set.
      ratexpset_t rs_;
      /// States to visit.
      std::stack<ratexp_t> todo_;
      /// ratexp -> state.
      smap map_;
    };
  }

  /// A ratexp automaton as a shared pointer.
  template <typename Aut>
  using ratexp_automaton
    = std::shared_ptr<detail::ratexp_automaton_impl<Aut>>;

}

#endif // !VCSN_CORE_RATEXP_AUTOMATON_HH
