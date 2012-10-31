#ifndef VCSN_CORE_AUTOMATON_HH
# define VCSN_CORE_AUTOMATON_HH

# include <memory> // shared_ptr
# include <string>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    class abstract_automaton
    {
    public:
      virtual const context& abstract_context() const = 0;
      virtual ~abstract_automaton() {}
      /// A key to recognize the type of the automaton.
      virtual std::string vname() const = 0;
    };

    using automaton = std::shared_ptr<abstract_automaton>;
  }
}

#endif // !VCSN_CORE_AUTOMATON_HH
