#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      static
      Registry<left_mult_t>&
      left_mult_registry()
      {
        static Registry<left_mult_t> instance{"left_mult"};
        return instance;
      }

      bool left_mult_register(const std::string& lctx, const std::string& rctx,
                              left_mult_t fn)
      {
        return left_mult_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    left_mult(const automaton& aut, const weight& w)
    {
      return detail::left_mult_registry().call(aut->vname(false) + " x " + w->vname(false),
                                               aut, w);
    }
  }
}
