// We really want assert to do its job.
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
  // automata: begin
  void
  automata()
  {
    using namespace vcsn::dyn;
    // A simple automaton.
    auto a1 = make_automaton("context = lal, q\n"
                             "$ 0 <1/2>\n"
                             "0 1 <2>a, <6>b\n"
                             "1 $\n", "daut");
    // Its context.
    auto ctx = context_of(a1);

    // Evaluate it.
    assert(evaluate(a1, make_word(ctx, "a"))
           == make_weight(ctx, "1"));
    assert(evaluate(a1, make_word(ctx, "b"))
           == make_weight(ctx, "3"));

    // Concatenate to itself.
    auto a2 = a1 * a1;
    assert(evaluate(a2, make_word(ctx, "ab"))
           == make_weight(ctx, "3"));
    assert(evaluate(a2, make_word(ctx, "bb"))
           == make_weight(ctx, "9"));

    // Self-conjunction, aka "power 2".
    auto a3 = a1 & a1;
    assert(evaluate(a3, make_word(ctx, "a"))
           == make_weight(ctx, "1"));
    assert(evaluate(a3, make_word(ctx, "b"))
           == make_weight(ctx, "9"));
  }
  // automata: end

  // expressions: begin
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
  // expressions: end

  // labels: begin
  void
  labels()
  {
    using namespace vcsn::dyn;
    // Labels are words, not letters.
    auto ctx = make_context("law, q");

    auto a = make_label(ctx, "a");
    auto b = make_label(ctx, "b");
    auto abba = make_label(ctx, "abba");

    // Labels can be compared.
    assert(a == a);
    assert(a != b);
    assert(a < b);
    assert(a <= b);
    assert(b > a);
    assert(b >= a);

    // This is the lexicographical order.
    assert(a < abba);
    assert(abba < b);

    // They also support multiplication (i.e., concatenation) when
    // possible: concatenation of labels-are-letters does not work, of
    // course.  That's why the context here is using labels-are-words.
    assert(a * b * b * a == abba);
  }
  // labels: end

  // weights: begin
  void
  weights()
  {
    using namespace vcsn::dyn;

    // This context specifies that weights are rational numbers (ℚ).
    auto ctx = make_context("lal, q");

    auto one = make_weight(ctx, "1");
    auto two = make_weight(ctx, "2");
    auto half = make_weight(ctx, "1/2");

    // Weights can be compared.
    assert(one != two);
    assert(one == one);
    assert(one < two);
    assert(one <= two);
    assert(two > one);
    assert(two >= one);

    // 1+1 = 2, etc.
    assert(one + one == two);
    assert(half + half == one);

    // 1/2 * 2 = 1.
    assert(half * two == one);

    // (1/2)* = 2.
    assert(half * -1 == two);
  }
  // weights: end
}

int
main(int, const char* argv[])
try
  {
    automata();
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
