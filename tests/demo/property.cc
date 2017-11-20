#include <iostream> // std::cout

#include <vcsn/algos/focus.hh>            // focus_automaton
#include <vcsn/algos/is-deterministic.hh> // is_deterministic
#include <vcsn/core/function-property.hh> // function_prop
#include <vcsn/core/mutable-automaton.hh> // automaton
#include <vcsn/core/property.hh>          // properties
#include <vcsn/ctx/lal_char_b.hh>         // a context
#include <vcsn/labelset/tupleset.hh>      // for the transducer
#include <vcsn/weightset/b.hh>            // weightset b

/**
 * Example of the properties usage.
 *
 * Automata have a cache of properties. Currently, there are 3 boolean
 * properties which are cached: `is_proper`, `is_deterministic`, `is_free`.
 */

using automaton_t = vcsn::mutable_automaton<vcsn::ctx::lal_char_b>;

namespace vcsn
{
  /**
   * It is possible to define new properties.
   *
   * Each property is defined by a tag: a struct (suffixed by '_ptag'),
   * defining the type of the property and its name.
   * A property can, across call to algorithms, be:
   *   - invalidated, its value is considered unknown or undefined,
   *   - preserved, its value did not change after the algorithm,
   *   - or defined, its value is updated.
   *
   * For example, after a call to `proper`, the property `is_proper` is set to
   * true.
   */

  // Define a new property.
  CREATE_PROP(is_happy, bool);
  CREATE_PROP(a_number, int);

  // What should be done by a function or algorithm is defined by a struct
  // called `function_prop` templated on a function tag (suffixed by '_ftag').
  // It defines:
  //  - whether all values must be invalidated first,
  //  - in case of invalidation, which properties should be ignored,
  //  - values to define for some properties.
  //
  // The `function_prop<proper_ftag>` states that all properties will be
  // invalidated but `is_proper` will be defined to true. All of this can be
  // changed (shown later).

  // Define a tag for invalidate_function. By default, properties will be
  // invalidated.
  CREATE_FUNCTION_TAG(invalidate);

  // Define a tag for preserve_function. KEEP_PROPERTIES allows to preserve
  // properties.
  CREATE_FUNCTION_TAG(preserve);
  KEEP_PROPERTIES(preserve);

  // To properly update the cache, the function needs to call, at the end,
  // the `update` method of the cache with the corresponding function tag.
  namespace
  {
    void invalidate_function(automaton_t aut)
    {
      aut->properties().update(invalidate_ftag{});
    }

    void preserve_function(automaton_t aut)
    {
      aut->properties().update(preserve_ftag{});
    }
  }
}

namespace
{
  void single_tape()
  {
    using namespace vcsn;

    std::cout << "# Properties on a single tape automaton.\n";

    /**
     * Automaton creation.
     */

    // Create the context: lal_char, b.
    auto al = set_alphabet<char_letters>{'a', 'b'};
    auto ctx = ctx::lal_char_b{al};

    // Create an empty automaton, from the context.
    auto aut = make_mutable_automaton(ctx);

    // Making: --> p --a--> q -->
    {
      auto p = aut->new_state();
      auto q = aut->new_state();
      aut->set_initial(p);
      aut->new_transition(p, q, ctx.labelset()->value('a'));
      aut->set_final(q);
    }

    /**
     * Behavior of properties across function calls.
     */

    // Display `is_deterministic` value in cache.
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';

    /**
     * Some functions like `is_deterministic` only compute the property value,
     * and put it in cache.
     */

    // Define `is_deterministic` value.
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';

    // Does not change `is_deterministic` value.
    preserve_function(aut);
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';

    // Undefine `is_deterministic`.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';

    /**
     * Cache interaction.
     */

    // Values can be put by hand in the cache.
    aut->properties().put(*aut, is_happy_ptag{}, true);

    // It is possible to check if a property is defined.
    if (aut->properties().is_unknown(*aut, a_number_ptag{}))
      std::cout << "No value for a_number.\n";
    else
      std::cout << "There is a value for a_number.\n";

    // Can be retrieved too.
    aut->properties().put(*aut, a_number_ptag{}, 10);
    auto num = aut->properties().get(*aut, a_number_ptag{});
    std::cout << "Retrieve a_number value = " << num << '\n';

    /**
     *  Changed properties of a function can be customized.
     */

    // Undefine is_happy.
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';
    invalidate_function(aut);
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';

    // Specify that invalidate_function defines is_happy to false.
    add_update(invalidate_ftag{}, is_happy_ptag{}, false);

    // Define is_happy.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';

    // Specify that preserve_function invalidates is_happy.
    add_invalidate(preserve_ftag{}, is_happy_ptag{});

    // Invalidate is_happy.
    preserve_function(aut);
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';

    // Specify that invalidate_function preserves a_number.
    add_preserve(invalidate_ftag{}, a_number_ptag{});

    // Preserve a_number.
    aut->properties().put(*aut, a_number_ptag{}, 12);
    aut->properties().print_prop(*aut, a_number_ptag{}) << '\n';
    invalidate_function(aut);
    aut->properties().print_prop(*aut, a_number_ptag{}) << '\n';

    // Remove preservation of a_number.
    remove_preserve(invalidate_ftag{}, a_number_ptag{});

    // No value is defined for a_number so it is invalidated.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, a_number_ptag{}) << '\n';

    // Define is_happy.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';

    // Remove update of is_happy.
    remove_update(invalidate_ftag{}, is_happy_ptag{});

    // is_happy is not preserved so invalidate is_happy.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, is_happy_ptag{}) << '\n';

    // All defined and ignored (for invalidation) properties can be cleared.
    // invalidate_function will retrieve its original behavior.
    clear_updated(invalidate_ftag{});
    clear_ignored(invalidate_ftag{});

    // Set a_number.
    aut->properties().put(*aut, a_number_ptag{}, 18);
    aut->properties().print_prop(*aut, a_number_ptag{}) << '\n';

    // Remove invalidation of a_number.
    remove_invalidate(invalidate_ftag{}, a_number_ptag{});

    // No value is defined to a_number so preserve it.
    invalidate_function(aut);
    aut->properties().print_prop(*aut, a_number_ptag{}) << '\n';
  }


  void double_tape()
  {
    using namespace vcsn;

    std::cout << "# Properties on a double tape automaton.\n";

    /**
     * Automaton creation.
     */

    // Context of the automaton: lat<lal_char, lal_char>, b.
    using letterset_t = letterset<set_alphabet<char_letters>>;
    using labelset_t = tupleset<letterset_t, letterset_t>;
    auto ls = letterset_t{'a', 'b'};
    auto ts = labelset_t{ls, ls};
    using context_t = context<labelset_t, b>;
    auto ctx = context_t{ts};

    // Making: --> p --a|a, a|b--> q -->
    auto aut = make_mutable_automaton(ctx);
    {
      auto p = aut->new_state();
      auto q = aut->new_state();
      aut->set_initial(p);
      aut->new_transition(p, q, ctx.labelset()->tuple('a', 'a'));
      aut->new_transition(p, q, ctx.labelset()->tuple('a', 'b'));
      aut->set_final(q);
    }

    /**
     * There is special property `on_tape` templated on the number of the
     * focused tape and a property. When working on a cache with a
     * focus_automaton, it is those `on_tape` properties that are modified or
     * fetched. Beware, the update of a cache does not take that into account.
     */

    // Display `is_deterministic` value in cache for the automaton and its first
    // tape.
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';
    aut->properties().print_prop(*aut, on_tape<0, is_deterministic_ptag>{})
      << '\n';

    // Define `is_deterministic` value.
    is_deterministic(aut);
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';
    aut->properties().print_prop(*aut, on_tape<0, is_deterministic_ptag>{})
      << '\n';

    // Define `is_deterministic` value for first tape.
    auto f0 = focus<0>(aut);
    is_deterministic(f0);
    aut->properties().print_prop(*aut, is_deterministic_ptag{}) << '\n';
    aut->properties().print_prop(*aut, on_tape<0, is_deterministic_ptag>{})
      << '\n';
    f0->properties().print_prop(*f0, is_deterministic_ptag{}) << '\n';
  }
}

int main()
{
  single_tape();
  std::cout << '\n';
  double_tape();
}
