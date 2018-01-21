#include <iostream>

#include <lib/vcsn/dyn/ctx-parser.hh>

// Include this one last, as it defines a macro `V`, which is used as
// a template parameter in boost/unordered/detail/allocate.hpp.
#include "tests/unit/test.hh"

static int
check_translate_context()
{
  int nerrs = 0;

#define CHECK(In, Out)                                  \
  ASSERT_EQ(Out, vcsn::ast::translate_context(In))

  CHECK("lao, q",        "oneset, q");

  CHECK("lal, b",          "letterset<char_letters>, b");
  CHECK("<char>[x], b",    "letterset<char_letters(x)>, b");
  CHECK("lal<char>, b",    "letterset<char_letters>, b");
  CHECK("lal<char(x)>, b", "letterset<char_letters(x)>, b");
  CHECK("lal(a), b",       "letterset<char_letters(a)>, b");
  CHECK("lal(), b",        "letterset<char_letters>, b");
  CHECK("[a]-> b",         "letterset<char_letters(a)>, b");

  CHECK("[\\]]-> b",       "letterset<char_letters(\\])>, b");
  CHECK("lal(\\)), b",     "letterset<char_letters(\\))>, b");
  CHECK("lal(\\\\), b",    "letterset<char_letters(\\\\)>, b");

  CHECK("lal<string>, b",  "letterset<string_letters>, b");

  CHECK("lal, expressionset<[x] -> b>(series)",
        "letterset<char_letters>, expressionset<letterset<char_letters(x)>, b>(series)");
  CHECK("lal, seriesset<[x] -> b>",
        "letterset<char_letters>, expressionset<letterset<char_letters(x)>, b>(series)");

  CHECK("law, zmin",     "wordset<char_letters>, zmin");
  CHECK("law(a), zmin",  "wordset<char_letters(a)>, zmin");
  CHECK("[a-z]*-> zmin", "wordset<char_letters(a-z)>, zmin");

  CHECK("lat<[a], [b], [c]> -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");
#undef CHECK
  return nerrs;
}

int main()
{
  int nerrs = 0;
  nerrs += check_translate_context();
  return !!nerrs;
}
