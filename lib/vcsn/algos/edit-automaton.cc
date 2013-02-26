#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  Registry<make_automaton_editor_t>&
  make_automaton_editor_registry()
  {
    static Registry<make_automaton_editor_t> instance{"make_automaton_editor"};
    return instance;
  }

  bool
  make_automaton_editor_register(const std::string& ctx,
                                 const make_automaton_editor_t& fn)
  {
    return make_automaton_editor_registry().set(ctx, fn);
  }

  automaton_editor*
  make_automaton_editor(const dyn::context& ctx)
  {
    return make_automaton_editor_registry().call(ctx->vname(),
                                                 ctx);
  }
}
