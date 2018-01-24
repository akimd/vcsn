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
  try                                                   \
    {                                                   \
      ASSERT_EQ(Out, vcsn::ast::translate_context(In)); \
    }                                                   \
  catch (std::exception& e)                             \
    {                                                   \
      std::cerr << "Failed: " << e.what() << '\n';      \
      ++nerrs;                                          \
    }

  // ASCII.
  CHECK("lao, b",     "oneset, b");
  CHECK("lao, f2",    "oneset, f2");
  CHECK("lao, log",   "oneset, log");
  CHECK("lao, nmin",  "oneset, nmin");
  CHECK("lao, q",     "oneset, q");
  CHECK("lao, qmp",   "oneset, qmp");
  CHECK("lao, r",     "oneset, r");
  CHECK("lao, rmin",  "oneset, rmin");
  CHECK("lao, z",     "oneset, z");
  CHECK("lao, zmin",  "oneset, zmin");

  // UTF-8.
  CHECK("lao, 𝔹",     "oneset, b");
  CHECK("lao, 𝔽₂",    "oneset, f2");
  CHECK("lao, Log",   "oneset, log");
  CHECK("lao, ℕmin",  "oneset, nmin");
  CHECK("lao, ℚ",     "oneset, q");
  CHECK("lao, ℚmp",   "oneset, qmp");
  CHECK("lao, ℝ",     "oneset, r");
  CHECK("lao, ℝmin",  "oneset, rmin");
  CHECK("lao, ℤ",     "oneset, z");
  CHECK("lao, ℤmin",  "oneset, zmin");

  // lal.  No gens, open.
  CHECK("lal, b",          "letterset<char_letters>, b");
  CHECK("lal<char>, b",    "letterset<char_letters>, b");
  CHECK("[...], b",        "letterset<char_letters>, b");

  // lal, with gens, closes.
  CHECK("lal(), b",        "letterset<char_letters()>, b");
  CHECK("[] -> b",         "letterset<char_letters()>, b");
  CHECK("<char>[a], b",    "letterset<char_letters(a)>, b");
  CHECK("lal<char(a)>, b", "letterset<char_letters(a)>, b");
  CHECK("lal(a), b",       "letterset<char_letters(a)>, b");
  CHECK("[a]-> b",         "letterset<char_letters(a)>, b");
  CHECK("[a]?-> b",        "letterset<char_letters(a)>, b");

  // [...] and escapes.
  CHECK("[\\[\\]()]-> b",  "letterset<char_letters(\\[\\]\\(\\))>, b");

  // lal(...) and escapes.
  CHECK("lal(\\)), b",     "letterset<char_letters(\\))>, b");
  CHECK("lal(\\\\), b",    "letterset<char_letters(\\\\)>, b");
  CHECK("lal([]{}\\(\\\\\\)), b", "letterset<char_letters([]{}\\(\\\\\\))>, b");

  CHECK("lal<string>, b",  "letterset<string_letters>, b");

  // Expressionset as a weightset.
  CHECK("lao, RatE[[x] -> b]",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, expressionset<[x] -> b>",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, expressionset<[x] -> b>(series)",
        "oneset, expressionset<letterset<char_letters(x)>, b>(series)");
  CHECK("lao, seriesset<[x] -> b>",
        "oneset, expressionset<letterset<char_letters(x)>, b>(series)");

  // Polynomialset as a weightset.
  CHECK("lao, Poly[[x] -> b]",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");
  CHECK("lao, polynomialset<[x] -> b>",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");

  CHECK("law, zmin",     "wordset<char_letters>, zmin");
  CHECK("law(a), zmin",  "wordset<char_letters(a)>, zmin");
  CHECK("[a-z]*-> zmin", "wordset<char_letters(a-z)>, zmin");

  CHECK("lat<[a], [b], [c]> -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");
  CHECK("[a] x [b] x [c] -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");

  CHECK("[a] -> lat<nmin, zmin, rmin>",
        "letterset<char_letters(a)>, lat<nmin, zmin, rmin>");
  CHECK("[a] -> nmin x zmin x rmin",
        "letterset<char_letters(a)>, lat<nmin, zmin, rmin>");

  // Using the UTF8 characters '×' and '→' instead of 'x' and '->'.
  CHECK("[a] × [b] × [c] → nmin × zmin × rmin",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, "
	"lat<nmin, zmin, rmin>");

#undef CHECK
  return nerrs;
}

int main()
{
  int nerrs = 0;
  nerrs += check_translate_context();
  return !!nerrs;
}
