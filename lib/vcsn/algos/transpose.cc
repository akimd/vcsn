#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/transpose.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  Registry<transpose_t>&
  transpose_registry()
  {
    static Registry<transpose_t> instance{"transpose"};
    return instance;
  }

  bool
  transpose_register(const std::string& ctx, const transpose_t& fn)
  {
    return transpose_registry().set(ctx, fn);
  }

  abstract_mutable_automaton*
  transpose(abstract_mutable_automaton& aut)
  {
    return transpose_registry().call(aut.vname(),
                                     aut);
  }
}
