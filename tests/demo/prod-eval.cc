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
    static automaton read_automaton(const std::string& f)
    {
      auto is = open_input_file(f);
      return read_automaton(*is);
    }

    static label read_word(const std::string& w, const context& ctx)
    {
      std::istringstream is{w};
      auto res = read_label(is, make_word_context(ctx));
      if (is.peek() != -1)
        vcsn::fail_reading(is, "unexpected trailing characters");
      return res;
    }
  }

  template <typename Aut>
  Aut
  read_automaton(const std::string& f)
  {
    dyn::automaton res = dyn::read_automaton(f);
    // Automaton typename.
    auto vname = res->vname();
    require(vname == Aut::element_type::sname(),
            f, ": invalid context: ", vname,
            ", expected: ", Aut::element_type::sname());
    auto& r = res->as<Aut>();
    return std::move(r);
  }

  template <typename Ctx>
  typename Ctx::labelset_t::word_t
  read_word(const std::string& w, const Ctx& ctx)
  {
    auto c = make_word_context(ctx);
    std::istringstream is{w};
    auto res = read_label(is, make_word_context(ctx));
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
    return res;
  }
}

template <typename Ctx>
void
sta_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using automaton_t = vcsn::mutable_automaton<Ctx>;
  automaton_t l = vcsn::read_automaton<automaton_t>(lhs);
  automaton_t r = vcsn::read_automaton<automaton_t>(rhs);
  automaton_t prod = vcsn::product<automaton_t, automaton_t>(l, r)->strip();
  typename Ctx::labelset_t::word_t input
    = vcsn::read_word<Ctx>(word, prod->context());
  vcsn::weight_t_of<Ctx> w = vcsn::eval<automaton_t>(prod, input);
  prod->context().weightset()->print(w, std::cout);
}

static void
dyn_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using namespace vcsn::dyn;
  automaton l = read_automaton(lhs);
  automaton r = read_automaton(rhs);
  automaton prod = product(l, r);
  label input = read_word(word, context_of(prod));
  weight w = eval(prod, input);
  print(w, std::cout, "text");
}

int
main(int argc, const char* argv[])
try
  {
    assert(argc == 4); (void) argc;
    dyn_prod_eval(argv[1], argv[2], argv[3]);
    std::cout << ",";
    sta_prod_eval<vcsn::ctx::lal_char_z>(argv[1], argv[2], argv[3]);
    std::cout << std::endl;
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
