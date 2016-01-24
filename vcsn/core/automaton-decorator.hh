#pragma once

#include <vcsn/misc/memory.hh>
#include <vcsn/core/automaton.hh> // Automaton.
#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Aggregate an automaton, and forward calls to it.
    ///
    /// \tparam Aut
    ///   The type of the wrapped automaton.
    /// \tparam Context
    ///   The context we pretend to feature.
    template <Automaton Aut,
              typename Context = context_t_of<Aut>>
    class automaton_decorator
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;

      using context_t = Context;

      /// The automaton type, without shared_ptr.
      using element_type = typename automaton_t::element_type;

      /// The (shared pointer) type to use it we have to create an
      /// automaton of the same (underlying) type.
      template <typename Ctx = context_t>
      using fresh_automaton_t
        = typename element_type::template fresh_automaton_t<Ctx>;

      using kind_t = typename context_t::kind_t;

      using labelset_t = typename context_t::labelset_t;
      using labelset_ptr = typename context_t::labelset_ptr;
      using label_t = typename labelset_t::value_t;

      using weightset_t = typename context_t::weightset_t;
      using weightset_ptr = typename context_t::weightset_ptr;
      using weight_t = typename weightset_t::value_t;

      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

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

      /// The automaton we decorate.
      ///
      /// Does not strip recursively.
      automaton_t strip()
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

#define DEFINE(Name)                                                    \
      template <typename... Args>                                       \
      static constexpr                                                  \
      auto                                                              \
      Name(Args&&... args)                                              \
        -> decltype(element_type::Name(std::forward<Args>(args)...))    \
      {                                                                 \
        return element_type::Name(std::forward<Args>(args)...);         \
      }

      DEFINE(lazy_transition);
      DEFINE(null_state);
      DEFINE(null_transition);
      DEFINE(post);
      DEFINE(pre);
      DEFINE(sname);

#undef DEFINE

      /*--------.
      | const.  |
      `--------*/

#define DEFINE(Name)                                            \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) const                                \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
        return aut_->Name(std::forward<Args>(args)...);         \
      }

      DEFINE(all_in);
      DEFINE(all_out);
      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(dst_of);
      DEFINE(get_final_weight);
      DEFINE(get_initial_weight);
      DEFINE(get_transition);
      DEFINE(has_state);
      DEFINE(has_transition);
      DEFINE(is_final);
      DEFINE(is_initial);
      DEFINE(label_of);
      DEFINE(labelset);
      DEFINE(num_all_states);
      DEFINE(num_finals);
      DEFINE(num_initials);
      DEFINE(num_states);
      DEFINE(num_transitions);
      DEFINE(prepost_label);
      DEFINE(print);
      DEFINE(print_set);
      DEFINE(print_state);
      DEFINE(print_state_name);
      DEFINE(src_of);
      DEFINE(state_has_name);
      DEFINE(state_is_strict);
      DEFINE(state_is_strict_in);
      DEFINE(states);
      DEFINE(weight_of);
      DEFINE(weightset);

#undef DEFINE


      /*------------.
      | non const.  |
      `------------*/

#define DEFINE(Name)                                            \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args)                                      \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
        return aut_->Name(std::forward<Args>(args)...);         \
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
      DEFINE(set_lazy);
      DEFINE(set_lazy_in);
      DEFINE(set_initial);
      DEFINE(set_transition);
      DEFINE(set_weight);
      DEFINE(unset_final);
      DEFINE(unset_initial);

#undef DEFINE
    };
  }
}
