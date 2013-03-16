#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <memory> // shared_ptr

namespace vcsn
{

  // vcsn/rat/ratexpset.hh
  class abstract_ratexpset;

  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    class abstract_automaton;
    using automaton = std::shared_ptr<abstract_automaton>;

    // vcsn/dyn/context.hh.
    class abstract_context;
    using context = std::shared_ptr<const abstract_context>;

    // vcsn/dyn/ratexp.hh.
    namespace details
    {
      class abstract_ratexp;
    }

    using ratexp = std::shared_ptr<details::abstract_ratexp>;

    // vcsn/rat/ratexpset.hh
    using ratexpset = std::shared_ptr<const abstract_ratexpset>;
  }
} // namespace vcsn

#endif // !VCSN_DYN_FWD_HH
