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

  /*------------------------------.
  | Check the basic weightsets.   |
  `------------------------------*/
#define CHECK_WS(Ref, A, B)                     \
  CHECK("lao, " Ref,  "oneset, " Ref);          \
  CHECK("lao, " A,    "oneset, " Ref);          \
  CHECK("lao, " B,    "oneset, " Ref);

  CHECK_WS("b",    "𝔹",    "B");
  CHECK_WS("f2",   "𝔽₂",   "F2");
  CHECK_WS("log",  "Log",  "LOG");
  CHECK_WS("nmin", "ℕmin", "Nmin");
  CHECK_WS("q",    "ℚ",    "Q");
  CHECK_WS("qmp",  "ℚmp",  "Qmp");
  CHECK_WS("r",    "ℝ",    "R");
  CHECK_WS("rmin", "ℝmin", "Rmin");
  CHECK_WS("z",    "ℤ",    "Z");
  CHECK_WS("zmin", "ℤmin", "Zmin");
#undef CHECK_WS


  /*-------------------.
  | Check labelsets.   |
  `-------------------*/

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

  // No weightset: defaults to B.
  CHECK("lal",          "letterset<char_letters>, b");
  CHECK("lal<char>",    "letterset<char_letters>, b");
  CHECK("[...]",        "letterset<char_letters>, b");
  CHECK("[a]",          "letterset<char_letters(a)>, b");



  /*-----------------.
  | Expressionset.   |
  `-----------------*/

  // Expressionset as a weightset.
  CHECK("lao, RatE[[x] -> b]",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, RatE [  [x] -> b  ]",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, expressionset<[x] -> b>",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, expressionset  <  [x] -> b  >",
        "oneset, expressionset<letterset<char_letters(x)>, b>");
  CHECK("lao, expressionset<[x] -> b>(series)",
        "oneset, expressionset<letterset<char_letters(x)>, b>(series)");
  CHECK("lao, seriesset<[x] -> b>",
        "oneset, expressionset<letterset<char_letters(x)>, b>(series)");
  CHECK("lao, seriesset <[x] -> b>",
        "oneset, expressionset<letterset<char_letters(x)>, b>(series)");


  /*-----------------.
  | Polynomialset.   |
  `-----------------*/

  // Polynomialset as a weightset.
  CHECK("lao, Poly[[x] -> b]",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");
  CHECK("lao, Poly [[x] -> b]",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");
  CHECK("lao, polynomialset<[x] -> b>",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");
  CHECK("lao, polynomialset <[x] -> b>",
        "oneset, polynomialset<letterset<char_letters(x)>, b>");

  CHECK("law, zmin",     "wordset<char_letters>, zmin");
  CHECK("law(a), zmin",  "wordset<char_letters(a)>, zmin");
  CHECK("[a-z]*-> zmin", "wordset<char_letters(a-z)>, zmin");


  /*----------------------.
  | Cartesian products.   |
  `----------------------*/

  CHECK("lat<[a], [b], [c]> -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");
  CHECK("lat < [a], [b], [c] > -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");
  CHECK("[a] x [b] x [c] -> f2",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, f2");

  CHECK("[a] -> lat<nmin, zmin, rmin>",
        "letterset<char_letters(a)>, lat<nmin, zmin, rmin>");
  CHECK("[a] -> lat < nmin , zmin , rmin >",
        "letterset<char_letters(a)>, lat<nmin, zmin, rmin>");
  CHECK("[a] -> nmin x zmin x rmin",
        "letterset<char_letters(a)>, lat<nmin, zmin, rmin>");

  // Using the UTF8 characters '×' and '→' instead of 'x' and '->'.
  CHECK("[a] × [b] × [c] → nmin × zmin × rmin",
        "lat<letterset<char_letters(a)>, letterset<char_letters(b)>, letterset<char_letters(c)>>, "
        "lat<nmin, zmin, rmin>");

  // Parens.
  CHECK("([a] × [b]) × ([c] x [d]) → (b x q) x (nmin × zmin)",
        "lat<lat<letterset<char_letters(a)>, letterset<char_letters(b)>>"
        """"", lat<letterset<char_letters(c)>, letterset<char_letters(d)>>>"
        ", lat<lat<b, q>, lat<nmin, zmin>>");

#undef CHECK
  return nerrs;
}

int main()
{
  int nerrs = 0;
  nerrs += check_translate_context();
  return !!nerrs;
}
