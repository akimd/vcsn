#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(make_automaton_editor);

    automaton_editor*
    make_automaton_editor(const dyn::context& ctx)
    {
      return details::make_automaton_editor_registry().call(ctx->vname(),
                                                            ctx);
    }
  }
}
