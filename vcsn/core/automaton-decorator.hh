#ifndef VCSN_CORE_AUTOMATON_DECORATOR_HH
# define VCSN_CORE_AUTOMATON_DECORATOR_HH

# include <vcsn/misc/memory.hh>
# include <vcsn/ctx/traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Aggregate an automaton, and forward calls to it.
    template <typename Aut>
    class automaton_decorator
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;

    public:
      /// The (shared pointer) type to use it we have to create an
      /// automaton of the same (underlying) type.
      using automaton_nocv_t = typename automaton_t::element_type::automaton_nocv_t;

      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using kind_t = typename automaton_t::element_type::kind_t;

      using labelset_ptr = typename automaton_t::element_type::labelset_ptr;
      using weightset_ptr = typename automaton_t::element_type::weightset_ptr;

    public:
      automaton_decorator(automaton_t aut)
        : aut_{aut}
      {}

      automaton_decorator(const automaton_decorator& aut)
        : aut_(aut.aut_)
      {}

      automaton_decorator(const context_t& ctx)
        : aut_(make_shared_ptr<automaton_t>(ctx))
      {}

      automaton_decorator(automaton_decorator&& aut)
      {
        std::swap(aut_, aut.aut_);
      }

      automaton_decorator& operator=(automaton_decorator&& that)
      {
        if (this != &that)
          aut_ = std::move(that.aut_);
        return *this;
      }

      automaton_t
      strip()
      {
        return aut_;
      }

    protected:
      /// The wrapped automaton, possibly const.
      // Must be defined early to please decltype.
      automaton_t aut_;


    public:
      /*------------.
      | constexpr.  |
      `------------*/

# define DEFINE(Name)                                                   \
      template <typename... Args>                                       \
      static constexpr                                                  \
      auto                                                              \
      Name(Args&&... args)                                              \
        -> decltype(automaton_t::element_type::Name(std::forward<Args>(args)...))     \
      {                                                                 \
        return automaton_t::element_type::Name(std::forward<Args>(args)...);          \
      }

      DEFINE(null_state);
      DEFINE(null_transition);
      DEFINE(post);
      DEFINE(pre);
      DEFINE(sname);

#undef DEFINE

      /*--------.
      | const.  |
      `--------*/

# define DEFINE(Name)                                           \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) const                                \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
      return aut_->Name(std::forward<Args>(args)...);           \
    }

      DEFINE(all_in);
      DEFINE(all_out);
      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(dst_of);
      DEFINE(final_transitions);
      DEFINE(get_final_weight);
      DEFINE(get_initial_weight);
      DEFINE(get_transition);
      DEFINE(has_transition);
      DEFINE(has_state);
      DEFINE(in);
      DEFINE(initial_transitions);
      DEFINE(is_final);
      DEFINE(is_initial);
      DEFINE(label_of);
      DEFINE(labelset);
      DEFINE(num_all_states);
      DEFINE(num_finals);
      DEFINE(num_initials);
      DEFINE(num_states);
      DEFINE(num_transitions);
      DEFINE(out);
      DEFINE(outin);
      DEFINE(print_state);
      DEFINE(print_state_name);
      DEFINE(src_of);
      DEFINE(state_has_name);
      DEFINE(states);
      DEFINE(transitions);
      DEFINE(vname);
      DEFINE(weight_of);
      DEFINE(weightset);

# undef DEFINE


      /*------------.
      | non const.  |
      `------------*/

# define DEFINE(Name)                                           \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args)                                      \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
      return aut_->Name(std::forward<Args>(args)...);           \
      }

      DEFINE(add_final);
      DEFINE(add_initial);
      DEFINE(add_transition);
      DEFINE(add_transition_copy);
      DEFINE(add_weight);
      DEFINE(del_state);
      DEFINE(del_transition);
      DEFINE(lmul_weight);
      DEFINE(new_state);
      DEFINE(new_transition);
      DEFINE(new_transition_copy);
      DEFINE(rmul_weight);
      DEFINE(set_final);
      DEFINE(set_initial);
      DEFINE(set_transition);
      DEFINE(set_weight);
      DEFINE(unset_final);
      DEFINE(unset_initial);

# undef DEFINE
    };
  }
}

#endif // !VCSN_CORE_AUTOMATON_DECORATOR_HH
