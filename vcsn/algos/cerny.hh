#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>

namespace vcsn
{
  /*------------------------------.
  | Function tag and properties.  |
  `------------------------------*/

  CREATE_FUNCTION_TAG(cerny);

  template <>
  struct function_prop<cerny_ftag>
  {
    // New automaton, no need to invalidate.
    static const bool invalidate = false;

    static auto& updated_prop()
    {
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      static auto updated_prop = create_updated_prop(
        {
          // By definition, cerny creates a DFA.
          { is_deterministic_ptag::id(), boost::any{true} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

  /*--------.
  | cerny.  |
  `--------*/

  /// Cerny automata are automata whose synchronizing word length is always
  /// (n - 1)^2, the upper bound of the Cerny's conjecture.
  ///
  /// Their transition function d(q, l) is defined by:
  ///
  /// - (q + 1) % n  if l == a
  /// - q            if l != a and q != n - 1
  /// - 0            if l != a and q == n - 1

  template <typename Ctx>
  mutable_automaton<Ctx>
  cerny(const Ctx& ctx, unsigned num_states)
  {
    require(0 < num_states, "num_states must be > 0");

    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    std::vector<state_t> states;
    states.reserve(num_states);

    for (unsigned i = 0; i < num_states; ++i)
      states.push_back(res->new_state());

    for (unsigned i = 0; i < num_states; ++i)
      {
        bool la = true;
        for (auto l : ctx.labelset()->generators())
          {
            auto dest = (la || i == num_states - 1) ? (i + 1) % num_states : i;
            res->add_transition(states[i], states[dest], l,
                                ctx.weightset()->one());
            la = false;
          }
      }

    res->set_initial(states[0]);
    res->set_final(states[0]);
    res->properties().update(cerny_ftag{});

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename>
      automaton
      cerny(const context& ctx, unsigned num_states)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::cerny(c, num_states);
      }
    }
  }
}
