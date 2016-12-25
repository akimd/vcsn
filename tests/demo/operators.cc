// We really assert to do its job.
#undef NDEBUG
#include <iostream>
#include <stdexcept>
#include <string>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/raise.hh>

namespace
{
  void
  expressions()
  {
    using namespace vcsn::dyn;
    // A simple context.
    auto ctx = make_context("lal, q");

    // Create an expression, and pretty-print it.
    auto e1 = make_expression(ctx, "<2>a");
    assert(format(e1) == "<2>a");
    auto e2 = make_expression(ctx, "<3>b");

    assert(format(e1 + e2) == "<2>a+<3>b");
    assert(format(e1 * e2) == "<6>(ab)");

    // e * -1 denotes e to the -1, that is e*.
    auto e = ((e1 + e2) * -1) * e1 * (e1 + e2);
    assert(format(e) == "<2>((<2>a+<3>b)*a(<2>a+<3>b))");
  }

  void
  labels()
  {
    using namespace vcsn::dyn;
    // Labels are words, not letters.
    auto ctx = make_context("law, q");

    auto a = make_label(ctx, "a");
    auto b = make_label(ctx, "b");
    auto abba = make_label(ctx, "abba");

    // Labels support comparisons.
    assert(a < b);
    assert(a <= b);
    assert(b > a);
    assert(b >= a);
    assert(a == a);
    assert(a != b);

    // This is the lexicographical order.
    assert(a < abba);
    assert(abba < b);

    // They also support multiplication (i.e., concatenation) when
    // possible: concatenation of labels-are-letters does not work, of
    // course.  That's why the context here is using labels-are-words.
    assert(a * b * b * a == abba);
  }

  void
  weights()
  {
    using namespace vcsn::dyn;

    auto ctx = make_context("lal, q");

    auto one = make_weight(ctx, "1");
    auto two = make_weight(ctx, "2");
    auto half = make_weight(ctx, "1/2");

    // Operators can be used on weights.  Including `==`.

    // 1+1 = 2, etc.
    assert(one + one == two);
    assert(half + half == one);

    // 1/2 * 2 = 1.
    assert(half * two == one);

    // (1/2)* = 2.
    assert(half * -1 == two);
  }
}

int
main(int, const char* argv[])
try
  {
    expressions();
    labels();
    weights();
  }
catch (const std::exception& e)
  {
    std::cerr << argv[0] << ": " << e.what() << '\n';
    exit(EXIT_FAILURE);
  }
catch (...)
  {
    std::cerr << argv[0] << ": unknown exception caught\n";
    exit(EXIT_FAILURE);
  }
