#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/make-context.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  Registry<make_context_t>&
  make_context_registry()
  {
    static Registry<make_context_t> instance{"make_context"};
    return instance;
  }

  bool
  make_context_register(const std::string& ctx, const make_context_t& fn)
  {
    return make_context_registry().set(ctx, fn);
  }

  ctx::abstract_context*
  make_context(const std::string& name, const std::string& gens)
  {
    return make_context_registry().call(name, gens);
  }
}
