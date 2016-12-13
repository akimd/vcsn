#include <iostream>

#include <vcsn/algos/info.hh>
#include <vcsn/core/mutable-automaton.hh> // Aut, cache, properties.
#include <vcsn/ctx/lal_char_b.hh>

#include "tests/unit/test.hh"

using namespace vcsn;
using automaton_t = mutable_automaton<ctx::lal_char_b>;

namespace vcsn
{
  // We create two new tags.
  // Invalidate will by default invalidate all properties.
  CREATE_FUNCTION_TAG(invalidate);

  // Keep will by default preserve all properties.
  CREATE_FUNCTION_TAG(keep);
  KEEP_PROPERTIES(keep);
}

// Check that f will define p's value in aut's cache as `value`.
template <typename FTag, typename PTag, typename T>
static void
update_and_assert(FTag f, PTag p, automaton_t aut, T value, size_t& nerrs)
{
  aut->properties().update(f);
  auto res = aut->properties().get(*aut, p);
  ASSERT_EQ(value, res);
}

// Check that f won't change p's value in aut's cache.
template <typename FTag, typename PTag>
static void
update_and_assert_same(FTag f, PTag p, automaton_t aut, size_t& nerrs)
{
  auto value = aut->properties().get(*aut, p);
  update_and_assert(f, p, aut, value, nerrs);
}

// Check that f will not define p's value in aut's cache.
template <typename FTag, typename PTag>
static void
update_and_assert_empty(FTag f, PTag p, automaton_t aut, size_t& nerrs)
{
  aut->properties().update(f);
  auto res = aut->properties().is_unknown(*aut, p);
  ASSERT_EQ(true, res);
}

// Specify that f will put `value` as p's value in aut's cache and check it.
template <typename FTag, typename PTag, typename T>
static void
check_update(FTag f, PTag p, automaton_t aut, T value, size_t& nerrs)
{
  std::cout << "***UPDATE " << p.sname()
            << " TO "       << value
            << " BY "       << f.sname() << "***\n";

  add_update(f, p, value);
  update_and_assert(f, p, aut, value, nerrs);
  clear_updated(f);
}

// Specify that f will preserve p's value in aut's cache and check it.
template <typename FTag, typename PTag>
static void
check_preserve(FTag f, PTag p, automaton_t aut, size_t& nerrs)
{
  std::cout << "***PRESERVE " << p.sname()
            << " BY " << f.sname() << "***\n";

  add_preserve(f, p);
  if (aut->properties().is_unknown(*aut, p))
    update_and_assert_empty(f, p, aut, nerrs);
  else
    update_and_assert_same(f, p, aut, nerrs);
  clear_ignored(f);
}

// Specify that f will invalidate p's value in aut's cache and check it.
template <typename FTag, typename PTag, typename T>
static void
check_invalidate(FTag f, PTag p, automaton_t aut, T value, size_t& nerrs)
{
  std::cout << "***INVALIDATE " << p.sname()
            << " BY " << f.sname() << "***\n";

  aut->properties().put(*aut, p, value);
  add_invalidate(f, p);
  update_and_assert_empty(f, p, aut, nerrs);
  clear_updated(f);
}

// Specify that f won't update p's value in aut's cache and check it.
template <typename FTag, typename PTag, typename T>
static void
check_remove_update(FTag f, PTag p, automaton_t aut, T value,
                    size_t& nerrs)
{
  std::cout << "***REMOVE UPDATE " << p.sname()
            << " FOR " << f.sname() << "***\n";

  aut->properties().invalidate(*aut, p);
  add_update(f, p, value);
  remove_update(f, p);
  update_and_assert_empty(f, p, aut, nerrs);
}

// Specify that f won't preserve p's value in aut's cache and check it.
template <typename FTag, typename PTag, typename T>
static void
check_remove_preserve(FTag f, PTag p, automaton_t aut, T value, size_t& nerrs)
{
  std::cout << "***REMOVE PRESERVE " << p.sname()
            << " FOR " << f.sname() << "***\n";

  aut->properties().put(*aut, p, value);
  // Ensure no value is defined at update.
  clear_updated(f);
  add_preserve(f, p);
  remove_preserve(f, p);
  update_and_assert_empty(f, p, aut, nerrs);
}

// Specify that f won't invalidate p's value in aut's cache and check it.
template <typename FTag, typename PTag, typename T>
static void
check_remove_invalidate(FTag f, PTag p, automaton_t aut, T value, size_t& nerrs)
{
  std::cout << "***REMOVE INVALIDATE " << p.sname()
            << " FOR " << f.sname() << "***\n";

  aut->properties().put(*aut, p, value);
  // Ensure no value is defined at update.
  clear_updated(f);
  // Ensure invalidation for all functions.
  add_invalidate(f, p);
  remove_invalidate(f, p);
  update_and_assert(f, p, aut, value, nerrs);
  clear_ignored(f);
}

// Check add_update and remove_update.
static void
check_update_all(size_t& nerrs)
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::lal_char_b{al};

  automaton_t aut = vcsn::make_shared_ptr<automaton_t>(ctx);

  auto k = keep_ftag{};
  auto i = invalidate_ftag{};
  auto p = is_proper_ptag{};
  auto d = is_deterministic_ptag{};

  // is_proper.
  check_update(k, p, aut, true, nerrs);
  check_update(k, p, aut, false, nerrs);
  check_update(i, p, aut, true, nerrs);
  check_update(i, p, aut, false, nerrs);
  check_remove_update(k, p, aut, true, nerrs);
  check_remove_update(i, p, aut, false, nerrs);

  // is_deterministic.
  check_update(k, d, aut, true, nerrs);
  check_update(k, d, aut, false, nerrs);
  check_update(i, d, aut, true, nerrs);
  check_update(i, d, aut, false, nerrs);
  check_remove_update(k, d, aut, false, nerrs);
  check_remove_update(i, d, aut, true, nerrs);
}

// Check add_preserve and remove_preserve.
static void
check_preserve_all(size_t& nerrs)
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::lal_char_b{al};

  automaton_t aut = vcsn::make_shared_ptr<automaton_t>(ctx);

  auto k = keep_ftag{};
  auto i = invalidate_ftag{};
  auto p = is_proper_ptag{};
  auto d = is_deterministic_ptag{};

  // Non-empty values.

  // is_proper.
  aut->properties().put(*aut, p, true);
  check_preserve(k, p, aut, nerrs);
  check_preserve(i, p, aut, nerrs);
  check_remove_preserve(i, p, aut, false, nerrs);
  check_remove_preserve(k, p, aut, true, nerrs);

  // is_deterministic.
  aut->properties().put(*aut, d, false);
  check_preserve(k, d, aut, nerrs);
  check_preserve(i, d, aut, nerrs);
  check_remove_preserve(i, d, aut, true, nerrs);
  check_remove_preserve(k, d, aut, false, nerrs);

  // Empty values.

  // is_proper.
  aut->properties().invalidate(*aut, is_proper_ptag{});
  check_preserve(k, p, aut, nerrs);
  check_preserve(i, p, aut, nerrs);

  // is_deterministic.
  aut->properties().invalidate(*aut, is_deterministic_ptag{});
  check_preserve(k, d, aut, nerrs);
  check_preserve(i, d, aut, nerrs);
}

// Check add_invalidate and remove_invalidate.
static void
check_invalidate_all(size_t& nerrs)
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::lal_char_b{al};

  automaton_t aut = vcsn::make_shared_ptr<automaton_t>(ctx);

  auto k = keep_ftag{};
  auto i = invalidate_ftag{};
  auto p = is_proper_ptag{};
  auto d = is_deterministic_ptag{};

  // is_proper.
  check_invalidate(k, p, aut, false, nerrs);
  check_invalidate(i, p, aut, true, nerrs);
  check_remove_invalidate(k, p, aut, true, nerrs);
  check_remove_invalidate(i, p, aut, false, nerrs);

  // is_deterministic.
  check_invalidate(k, d, aut, true, nerrs);
  check_invalidate(i, d, aut, false, nerrs);
  check_remove_invalidate(k, d, aut, false, nerrs);
  check_remove_invalidate(i, d, aut, true, nerrs);
}

int
main()
{
  size_t nerrs = 0;
  check_update_all(nerrs);
  check_preserve_all(nerrs);
  check_invalidate_all(nerrs);
  return !!nerrs;
}
