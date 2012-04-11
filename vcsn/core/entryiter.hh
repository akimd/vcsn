#ifndef VCSN_CORE_ENTRYITER_HH
# define VCSN_CORE_ENTRYITER_HH

#include <set>
#include <utility>

namespace
{
  template <class Automaton, class C>
  class entry_iterator
  {
    typedef typename Automaton::state_t state_t;
    typedef typename Automaton::transition_t transition_t;
    typedef typename std::pair<state_t, state_t> entry_pos_t;

    typedef std::function<bool(typename C::value_type)> predicate_t;
    typedef boost::filter_iterator<predicate_t,
				   typename C::const_iterator>
              const_iterator;

    const Automaton& a_;
    std::set<entry_pos_t> seen_;
    const C cont_;
    predicate_t predicate_;
  public:
    entry_iterator(const Automaton& a, const C&& cont)
      : a_(a), cont_(cont), predicate_([&] (transition_t i) {
	// Process each position once.
	entry_pos_t pos { a.src_of(i), a.dst_of(i) };
	return seen_.insert(pos).second;
      })
    {
    }

    const_iterator begin() const
    {
      return const_iterator(predicate_, cont_.begin(), cont_.end());
    }

    const_iterator end() const
    {
      return const_iterator(predicate_, cont_.end(), cont_.end());
    }

  };

}

#endif // !VCSN_CORE_ENTRYITER_HH
