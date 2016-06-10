// demaille.13.ciaa
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace dyn
  {
    /// Dyn version of the read_automaton function.
    ///
    /// Read an automaton without knowing its context.
    static automaton read_automaton(const std::string& f)
    {
      auto is = open_input_file(f);
      return read_automaton(*is);
    }

    static label read_word(const context& ctx, const std::string& w)
    {
      std::istringstream is{w};
      auto res = read_label(make_word_context(ctx), is);
      require(is.peek() == EOF, "unexpected trailing characters: ", is);
      return res;
    }
  }

  /// Static version of the read_automaton function.
  ///
  /// Read an automaton with a specified context (from the Aut template parameter).
  template <Automaton Aut>
  Aut
  read_automaton(const std::string& f)
  {
    dyn::automaton res = dyn::read_automaton(f);
    // Automaton typename.
    auto vname = res->vname();
    VCSN_REQUIRE(vname == Aut::element_type::sname(),
                 f, ": invalid context: ", vname,
                 ", expected: ", Aut::element_type::sname());
    return std::move(res->as<Aut>());
  }

  template <typename Ctx>
  word_t_of<Ctx>
  read_word(const Ctx& ctx, const std::string& w)
  {
    auto c = make_word_context(ctx);
    std::istringstream is{w};
    auto res = read_label(make_word_context(ctx), is);
    require(is.peek() == EOF, "unexpected trailing characters: ", is);
    return res;
  }
}

/// Static implementation of the prod_eval.
///
/// Read two automata and a word. Compute the product of these automata and
/// evaluate the word from it.
/// \param Ctx the specified context of the automata and the word. Each of them
/// have to be from these context or the program will fail.
template <typename Ctx>
void
sta_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using namespace vcsn;
  // The automata's exact type.
  using automaton_t = mutable_automaton<Ctx>;
  // Left automaton, here the binary decoder.
  automaton_t l = read_automaton<automaton_t>(lhs);
  // Right automaton, here the automaton accepting only even numbers.
  automaton_t r = read_automaton<automaton_t>(rhs);
  // The product automaton, accepting even numbers and outputing them as
  // decimal numbers. Needs to be stripped if we want to retrieve the same type
  // of automata as l and r.
  automaton_t prod = conjunction<automaton_t, automaton_t>(l, r)->strip();
  // The word to evaluate in prod.
  word_t_of<Ctx> input = read_word<Ctx>(prod->context(), word);
  // The result weight from the evaluation corresponding to its decimal value,
  weight_t_of<Ctx> w = eval<automaton_t>(prod, input);
  // Display of the result, we need to use the automaton's weightset to be able
  // to print the weight as the print function cannot be generic in static.
  prod->context().weightset()->print(w, std::cout);
}

/// Dyn implementation of the prod_eval.
///
/// Read two automata and a word. Compute the product of these automata and
/// evaluate the word from it. The context does not have to be specified as
/// the context of the parameters is computed dynamically.
static void
dyn_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using namespace vcsn::dyn;
  using vcsn::dyn::label;
  // We don't need the automata's exact type.
  // Left automaton, here the binary decoder.
  automaton l = read_automaton(lhs);
  // Right automaton, here the automaton accepting only even numbers.
  automaton r = read_automaton(rhs);
  // The product automaton, accepting even numbers and outputing them as
  // decimal numbers.
  automaton prod = conjunction(l, r);
  // The word to evaluate in prod.
  label input = read_word(context_of(prod), word);
  // The result weight from the evaluation corresponding to its decimal value,
  weight w = eval(prod, input);
  // Display of the result, no need to use the weightset as the dynamic print
  // is generic, the format is specified though.
  print(w, std::cout, "text");
}

int
main(int argc, const char* argv[])
try
  {
    vcsn::require(argc == 4, "not enough arguments");
    dyn_prod_eval(argv[1], argv[2], argv[3]);
    std::cout << ", ";
    sta_prod_eval<vcsn::ctx::lal_char_z>(argv[1], argv[2], argv[3]);
    std::cout << '\n';
  }
 catch (const std::exception& e)
   {
     std::cerr << argv[0] << ": " << e.what() << std::endl;
     exit(EXIT_FAILURE);
   }
 catch (...)
   {
     std::cerr << argv[0] << ": unknown exception caught" << std::endl;
     exit(EXIT_FAILURE);
   }
