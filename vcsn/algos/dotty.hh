#ifndef VCSN_ALGOS_DOTTY_HH
#define VCSN_ALGOS_DOTTY_HH

#include <iostream>
#include <unordered_map>

namespace vcsn
{
  template <class A>
  void
  dotty(A& aut, std::ostream& out)
  {
    typedef typename A::state_t state_t;
    std::unordered_map<state_t, unsigned> names;

    auto& w = aut.get_weights();

    bool show_unit = w.show_unit();

    out << "digraph {\n  rankdir=LR\n  node [shape=circle];\n";
    for (auto s : aut.states())
      {
	unsigned n = names[s] = names.size();
	out << "  " << n << "\n";
	if (aut.is_initial(s))
	  {
	    auto k = aut.get_initial_weight(s);
	    out << "  I" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  I" << n << " -> " << n;
	    if (show_unit || !w.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
	if (aut.is_final(s))
	  {
	    auto k = aut.get_final_weight(s);
	    out << "  F" << n
		<< " [style=invis,shape=none,label=\"\",width=0,height=0]\n";
	    out << "  " << n << " -> F" << n;
	    if (show_unit || !w.is_unit(k))
	      out << " [label=\"{" << k << "}\"]";
	    out << "\n";
	  }
      }

    for (auto t : aut.transitions())
      {
	unsigned src = names[aut.src_of(t)];
	unsigned dst = names[aut.dst_of(t)];

	out << "  " << src << " -> " << dst
	    << " [label=\"";
	auto k = aut.weight_of(t);
	if (show_unit || !w.is_unit(k))
	  out << "{" << k << "}";
	out << aut.label_of(t) << "\"]\n";
      }
    out << "}" << std::endl;
  }
}

#endif // VCSN_ALGOS_DOTTY_HH
