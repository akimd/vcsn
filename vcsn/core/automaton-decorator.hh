#ifndef VCSN_CORE_AUTOMATON_DECORATOR_HH
# define VCSN_CORE_AUTOMATON_DECORATOR_HH

namespace vcsn
{

  template <typename Aut>
  class automaton_decorator
  {
  public:
    /// The type of automaton to wrap.
    using automaton_t = Aut;
    /// The type of the automata to produce from this kind o
    /// automata.  For instance, determinizing a
    /// transpose_automaton<const mutable_automaton<Ctx>> should
    /// yield a transpose_automaton<mutable_automaton<Ctx>>, without
    /// the "inner" const.
    using self_nocv_t = automaton_decorator<typename automaton_t::self_nocv_t>;
    using context_t = typename automaton_t::context_t;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;
    using label_t = typename automaton_t::label_t;
    using weight_t = typename automaton_t::weight_t;

    using labelset_t = typename automaton_t::labelset_t;
    using weightset_t = typename automaton_t::weightset_t;
    using kind_t = typename automaton_t::kind_t;

    using labelset_ptr = typename automaton_t::labelset_ptr;
    using weightset_ptr = typename automaton_t::weightset_ptr;

  public:
    automaton_decorator(automaton_t& aut)
      : aut_{&aut}
    {}

    automaton_decorator(automaton_decorator&& aut)
    {
      std::swap(aut_, aut.aut_);
    }

    /// Forward constructor.
    template <typename... Args>
    automaton_decorator(Args&&... args)
      : aut_{new automaton_t{std::forward<Args>(args)...}}
    {}

    automaton_decorator& operator=(automaton_decorator&& that)
    {
      if (this != &that)
        *aut_ = std::move(*that.aut_);
      return *this;
    }

    automaton_t*
    original_automaton()
    {
      return aut_;
    }

    /// Used to workaround issues with decltype, see the const_casts
    /// above, and http://stackoverflow.com/questions/17111406.
    using automaton_nocv_t = typename std::remove_cv<automaton_t>::type;

  protected:
    /// The wrapped automaton, possibly const.
    // Must be defined early to please decltype.
    automaton_t* aut_;
  };

}

#endif // !VCSN_CORE_AUTOMATON_DECORATOR_HH
