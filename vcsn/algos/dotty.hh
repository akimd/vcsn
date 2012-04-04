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
    typedef typename A::state_t state_t;
    std::unordered_map<state_t, unsigned> names;

    auto& ws = aut.weightset();
    auto& es = aut.entryset();

    bool show_unit = ws.show_unit();

    out << "digraph A {\n  rankdir=LR\n  node [shape=circle];\n";

    // Name all states.
    for (auto s : aut.states())
      {
	unsigned n = names[s] = names.size();
	out << "  " << n << "\n";
      }

    for (auto t : aut.all_entries())
      {
	auto src = aut.src_of(t);
	auto dst = aut.dst_of(t);

	if (src == aut.pre())
	  {
	    unsigned n = names[dst];
	    auto k = aut.weight_of(t);
	    out << "  I" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  I" << n << " -> " << n;
	    if (show_unit || !ws.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
	else if (dst == aut.post())
	  {
	    unsigned n = names[src];
	    auto k = aut.weight_of(t);
	    out << "  F" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  " << n << " -> F" << n;
	    if (show_unit || !ws.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
	else
	  {
	    unsigned ns = names[src];
	    unsigned nd = names[dst];
	    out << "  " << ns << " -> " << nd
		<< " [label=\"";
	    str_escape(out, es.format(aut.entry_at(t))) << "\"]\n";
	  }
      }
    out << "}" << std::endl;
  }
}

#endif // VCSN_ALGOS_DOTTY_HH
