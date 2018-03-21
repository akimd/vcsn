#include <iostream> //std::cout

#include <vcsn/alphabets/char.hh>     // type of letter
#include <vcsn/alphabets/setalpha.hh> // alphabet
#include <vcsn/algos/standard.hh>     // vcsn::standard
#include <vcsn/algos/conjunction.hh>  // vcsn::conjunction
#include <vcsn/algos/daut.hh>         // vcsn::dautt
#include <vcsn/labelset/tupleset.hh>  // tupleset
#include <vcsn/labelset/letterset.hh> // letterset
#include <vcsn/weightset/q.hh>        // weightset Q

namespace
{
  /// Example usage of Vcsn's static layer.
  ///
  /// Creation of an automaton from an expression, self-conjunction, and
  /// display.
  void example(const std::string& expression)
  {
    using namespace vcsn;

    // Alphabet
    using alphabet_t = set_alphabet<char_letters>;

    // Letterset (labelset with a simple alphabet)
    using letterset_t = letterset<alphabet_t>;

    // Create the labelset.
    auto ls = letterset_t{'a', 'b', 'c', 'd', 'e'};

    // Context of the automaton: lat<lal_char, lal_char>, q
    using context_t = context<letterset_t, q>;

    // Create the context from the labelset.
    // No argument for the weightset, as it's just B.
    auto ctx = context_t{ls};

    // Create an expression from the user's argument.
    auto es = vcsn::make_expressionset(ctx);
    auto e = vcsn::conv(es, expression);

    // Print it.
    es.print(e, std::cout) << '\n';

    // Type of the automaton
    using automaton_t = mutable_automaton<context_t>;

    // Its standard automaton.
    auto a = vcsn::standard<automaton_t>(es, e);

    // Its conjunction with itself.
    auto c = vcsn::conjunction(a, a);

    // print it.
    vcsn::daut(c, std::cout) << '\n';
  }
}

int
main(int argc, const char* argv[])
try
  {
    vcsn::require(argc == 2,
                  "invalid number of arguments: ", argc);
    example(argv[1]);
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
