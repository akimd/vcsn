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

  protected:
    /// The wrapped automaton, possibly const.
    // Must be defined early to please decltype.
    automaton_t* aut_;

  public:
    /// The type of the automata to produce from this kind o
    /// automata.  For instance, determinizing a
    /// transpose_automaton<const mutable_automaton<Ctx>> should
    /// yield a transpose_automaton<mutable_automaton<Ctx>>, without
    /// the "inner" const.
    using self_nocv_t = automaton_decorator<typename automaton_t::self_nocv_t>;
    using context_t = context_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;
    using label_t = label_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;

    using labelset_t = labelset_t_of<automaton_t>;
    using weightset_t = weightset_t_of<automaton_t>;
    using kind_t = typename automaton_t::kind_t;

    using labelset_ptr = typename automaton_t::labelset_ptr;
    using weightset_ptr = typename automaton_t::weightset_ptr;

  public:
    automaton_decorator(automaton_t& aut)
      : aut_{&aut}
    {}

    automaton_decorator(automaton_t&& aut)
      : aut_{new automaton_t(std::move(aut))}
    {}

    automaton_decorator(automaton_t* aut)
      : aut_{aut}
    {}

    automaton_decorator(const automaton_decorator& aut)
      : aut_(aut.aut_)
    {}

    automaton_decorator(automaton_decorator&& aut)
    {
      std::swap(aut_, aut.aut_);
    }

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


    /*--------------------.
    | constexpr methods.  |
    `--------------------*/

# define DEFINE(Name)                                                   \
    template <typename... Args>                                         \
    static constexpr                                                    \
    auto                                                                \
    Name(Args&&... args)                                                \
      -> decltype(automaton_t::Name(std::forward<Args>(args)...))       \
    {                                                                   \
      return automaton_t::Name(std::forward<Args>(args)...);            \
    }

    DEFINE(null_state);
    DEFINE(null_transition);

#undef DEFINE

    /*----------------.
    | const methods.  |
    `----------------*/

# define DEFINE(Name)                                           \
    template <typename... Args>                                 \
    auto                                                        \
    Name(Args&&... args) const                                  \
      -> decltype(aut_->Name(std::forward<Args>(args)...))      \
    {                                                           \
      return aut_->Name(std::forward<Args>(args)...);           \
    }

    DEFINE(all_states);
    DEFINE(all_transitions);
    DEFINE(context);
    DEFINE(labelset);
    DEFINE(num_all_states);
    DEFINE(num_finals);
    DEFINE(num_initials);
    DEFINE(num_states);
    DEFINE(num_transitions);
    DEFINE(states);
    DEFINE(transitions);
    DEFINE(weightset);
# undef DEFINE


    /*--------------------------------.
    | Forwarded, does not transpose.  |
    `--------------------------------*/

# define DEFINE(Name)                                           \
    template <typename... Args>                                 \
    auto                                                        \
    Name(Args&&... args)                                        \
      -> decltype(aut_->Name(std::forward<Args>(args)...))      \
    {                                                           \
      return aut_->Name(std::forward<Args>(args)...);           \
    }

    DEFINE(del_state);
    DEFINE(new_state);
# undef DEFINE
  };

}

#endif // !VCSN_CORE_AUTOMATON_DECORATOR_HH
