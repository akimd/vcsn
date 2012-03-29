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
    typedef typename A::bare_state_t bare_state_t;
    std::unordered_map<bare_state_t, unsigned> names;

    auto& ws = aut.weightset();
    auto& al = aut.alphabet();

    bool show_unit = ws.show_unit();

    out << "digraph A {\n  rankdir=LR\n  node [shape=circle];\n";
    // FIXME: for (auto s : aut.states())
    for (auto s = aut.states().begin(); s != aut.states().end(); ++s)
      {
	unsigned n = names[*s] = names.size();
	out << "  " << n << "\n";
	if (aut.is_initial(s))
	  {
	    auto k = aut.get_initial_weight(s);
	    out << "  I" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  I" << n << " -> " << n;
	    if (show_unit || !ws.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
	if (aut.is_final(s))
	  {
	    auto k = aut.get_final_weight(s);
	    out << "  F" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  " << n << " -> F" << n;
	    if (show_unit || !ws.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
      }

    // FIXME: for (auto t : aut.transitions())
    for (auto t = aut.transitions().begin(); t != aut.transitions().end(); ++t)
      {
	unsigned src = names[*aut.src_of(t)];
	unsigned dst = names[*aut.dst_of(t)];

	out << "  " << src << " -> " << dst
	    << " [label=\"";
	auto k = aut.weight_of(t);
	if (show_unit || !ws.is_unit(k))
	  out << "{" << k << "}";
	str_escape(out, al.format(aut.label_of(t))) << "\"]\n";
      }
    out << "}" << std::endl;
  }
}

#endif // VCSN_ALGOS_DOTTY_HH
