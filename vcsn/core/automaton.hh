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
      virtual ~abstract_automaton() {}
      /// A description of the automaton, sufficient to build it.
      /// \param full  whether to include the genset.
      ///              if false, same as sname.
      virtual std::string vname(bool full = true) const = 0;
    };

    using automaton = std::shared_ptr<abstract_automaton>;
  }
}

#endif // !VCSN_CORE_AUTOMATON_HH
