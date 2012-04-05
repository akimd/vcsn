#ifndef VCSN_ALGOS_DOTTY_HH
#define VCSN_ALGOS_DOTTY_HH

#include <iostream>
#include <unordered_map>
#include "../misc/escape.hh"

namespace vcsn
{
  template <class A>
  void
  dotty(A& aut, std::ostream& out)
  {
    std::unordered_map<typename A::state_t, unsigned> names;

    out << "digraph A {\n  rankdir=LR\n  node [shape=circle];\n";

    // Name all states.
    for (auto s : aut.states())
      {
	unsigned n = names[s] = names.size();
	// We have to output them all, in case some of them have no
	// connection.
	out << "  " << n << "\n";
      }

    for (auto t : aut.all_entries())
      {
	auto src = aut.src_of(t);
	auto dst = aut.dst_of(t);

	if (src == aut.pre())
	  {
	    unsigned n = names[dst];
	    out << "  I" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  I" << n << " -> " << n;
	  }
	else if (dst == aut.post())
	  {
	    unsigned n = names[src];
	    out << "  F" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  " << n << " -> F" << n;
	  }
	else
	  {
	    unsigned ns = names[src];
	    unsigned nd = names[dst];
	    out << "  " << ns << " -> " << nd;
	  }
	std::string s = aut.entryset().format(aut.entry_at(t));
	if (!s.empty())
	  {
	    out << " [label=\"";
	    str_escape(out, s) << "\"]";
	  }
	out << "\n";
      }
    out << "}" << std::endl;
  }
}

#endif // VCSN_ALGOS_DOTTY_HH
