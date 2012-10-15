#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/standard_of.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  Registry<standard_of_t>&
  standard_of_registry()
  {
    static Registry<standard_of_t> instance{"standard_of"};
    return instance;
  }

  bool
  standard_of_register(const std::string& ctx, const standard_of_t& fn)
  {
    return standard_of_registry().set(ctx, fn);
  }

  dyn::abstract_automaton*
  standard_of(const ctx::abstract_context& ctx, const rat::exp_t e)
  {
    return standard_of_registry().call(ctx.vname(),
                                       ctx, e);
  }
}
