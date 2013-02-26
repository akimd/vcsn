#ifndef VCSN_DYN_AUTOMATON_HH
# define VCSN_DYN_AUTOMATON_HH

# include <memory> // shared_ptr
# include <string>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    /// Base class for automata.
    ///
    /// FIXME: Should not exist, we should model as we did for ratexp.
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

#endif // !VCSN_DYN_AUTOMATON_HH
