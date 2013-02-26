#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <memory> // shared_ptr

namespace vcsn
{

  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    class abstract_automaton;
    using automaton = std::shared_ptr<abstract_automaton>;

    // vcsn/dyn/ratexp.hh.
    namespace details
    {
      class abstract_ratexp;
    }

    using ratexp = std::shared_ptr<details::abstract_ratexp>;
  }
} // namespace vcsn

#endif // !VCSN_DYN_FWD_HH
