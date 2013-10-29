// demaille.13.ciaa
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <lib/vcsn/dot/driver.hh>

#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  template <typename Aut>
  Aut
  read_automaton_file(const std::string& f)
  {
    vcsn::detail::dot::driver d;
    auto res = d.parse_file(f);
    if (!d.errors.empty())
      throw std::runtime_error(d.errors);
    // Automaton typename.
    auto sname = vcsn::dyn::detail::context_base::sname(res->vname());
    if (sname != Aut::sname())
      throw std::runtime_error(f + ": invalid context: " + sname
                               + ", expected: " + Aut::sname());
    auto& r = res->as<Aut&>();
    return std::move(r);
  }
}

template <typename Ctx>
void
sta_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using automaton_t = vcsn::mutable_automaton<Ctx>;
  automaton_t l = vcsn::read_automaton_file<automaton_t>(lhs);
  automaton_t r = vcsn::read_automaton_file<automaton_t>(rhs);
  automaton_t prod = vcsn::product<automaton_t, automaton_t>(l, r);
  typename Ctx::weight_t w = vcsn::eval<automaton_t>(prod, word);
  prod.context().weightset()->print(std::cout, w);
}

static void
dyn_prod_eval(const std::string& lhs, const std::string& rhs,
              const std::string& word)
{
  using namespace vcsn::dyn;
  automaton l = read_automaton_file(lhs);
  automaton r = read_automaton_file(rhs);
  automaton prod = product(l, r);
  weight w = eval(prod, word);
  print(w, std::cout, "text");
}

int
main(int argc, const char* argv[])
try
  {
    assert(argc == 4);
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
