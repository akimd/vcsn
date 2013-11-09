// demaille.13.ciaa
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    static automaton read_automaton(const std::string& f)
    {
      auto is = open_input_file(f);
      return read_automaton(*is);
    }
  }

  template <typename Aut>
  Aut
  read_automaton(const std::string& f)
  {
    dyn::automaton res = dyn::read_automaton(f);
    // Automaton typename.
    auto sname = res->vname(false);
    if (sname != Aut::sname())
      throw std::runtime_error(f + ": invalid context: " + sname
                               + ", expected: " + Aut::sname());
    auto& r = res->as<Aut>();
    return std::move(r);
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
  automaton_t prod = vcsn::product<automaton_t, automaton_t>(l, r);
  typename Ctx::weight_t w = vcsn::eval<automaton_t>(prod, word);
  prod.context().weightset()->print(std::cout, w);
}

static void
dyn_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using namespace vcsn::dyn;
  automaton l = read_automaton(lhs);
  automaton r = read_automaton(rhs);
  automaton prod = product(l, r);
  weight w = eval(prod, word);
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
