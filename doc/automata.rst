.. default-domain:: cpp

Automata
========

Currently there is only one automaton class implemented in Vaucanson2.
It allows modifications, forward and backward traversal.   We will probably
split these features as separate concept, in the future.

An automaton may have several initial and final states, each with its
own initial and final weight.  Internally, these initial and final
weights are represented as weights on transitions leaving a
*pre-initial* state or reaching a *post-final* states.

Some people will want to consider these two special states are mostly
implementation detail, and work only on the states that really belong
to the automaton.  In some algorithm however it is easier to consider
these two states as part of the automaton.  The following interface
accommodate both situations.

The transition that leave the *pre-initial* state are called *initial
transition*.  The destinations of these transitions are called
*initial states*.  Similarly, transition reaching the *post-initial*
state are called *final transitions*, and their sources are the *final
states*.

.. Note::

   Do we want to expose the special character used to label initial and
   final transitions?


Overview
--------

Here is the interface of an automaton::

    typedef ... alphabet_t;
    typedef ... weightset_t;
    typedef ... kind_t;
    typedef ... entryset_t;

    typedef ... state_t;
    typedef ... transition_t;

    typedef ... label_t;
    typedef ... weight_t;
    typedef ... entry_t;

    // Related sets
    const alphabet_t&  alphabet() const;
    const weightset_t& weightset() const;
    const entryset_t&  entryset() const;

    // Special states and transitions
    state_t      pre() const;
    state_t      post() const;
    state_t      invalid_state() const;
    transition_t invalid_transition() const;

    // Statistics
    size_t nb_states() const;
    size_t nb_initials() const;
    size_t nb_finals() const;
    size_t nb_transitions() const;

    // Queries on states
    bool has_state(state_t s) const;
    bool is_initial(state_t s) const;
    bool is_final(state_t s) const;
    weight_t get_initial_weight(state_t s) const;
    weight_t get_final_weight(state_t s) const;

    // Queries on transitions
    transition_t get_transition(state_t src, state_t dst, label_t l) const;
    bool has_transition(state_t src, state_t dst, label_t l) const;
    bool has_transition(transition_t t) const;

    state_t src_of(transition_t t) const;
    state_t dst_of(transition_t t) const;
    label_t label_of(transition_t t) const;
    weight_t weight_of(transition_t t) const;

    alphabet_t::word_t word_label_of(transition_t t) const;

    // Edition of states
    state_t new_state();
    void del_state(state_t s);

    void set_initial(state_t s, weight_t k);
    void set_initial(state_t s);
    weight_t add_initial(state_t s, weight_t k);
    void unset_initial(state_t s);

    void set_final(state_t s, weight_t k);
    void set_final(state_t s);
    weight_t add_final(state_t s, weight_t k);
    void unset_final(state_t s);

    // Edition of transitions
    void del_transition(transition_t t);
    void del_transition(state_t src, state_t dst, label_t l);
    transition_t set_transition(state_t src, state_t dst, label_t l, weight_t k);
    transition_t set_transition(state_t src, state_t dst, label_t l);
    weight_t add_transition(state_t src, state_t dst, label_t l, weight_t k);
    weight_t add_transition(state_t src, state_t dst, label_t l);

    weight_t set_weight(transition_t t, weight_t k);
    weight_t add_weight(transition_t t, weight_t k);

    // Iteration on states and transitions
    // (transition_container and state_container are fictive types)
    state_container states() const;
    state_container all_states() const;
    transition_container transitions() const;
    transition_container all_transitions() const;

    transition_container initials() const;
    transition_container finals() const;

    transition_container out(state_t s) const;
    transition_container all_out(state_t s) const;
    transition_container out(state_t s, const label_t& l) const;

    transition_container in(state_t s) const;
    transition_container all_in(state_t s) const;
    transition_container in(state_t s, const label_t& l) const;

    transition_container outin(state_t s, state_t d) const;

    // Iteration on entries
    transition_container entries() const;
    transition_container all_entries() const;
    entry_t entry_at(state_t s, state_t d) const;
    entry_t entry_at(transition_t t) const;


Detailed interface
------------------

Types
~~~~~

.. type:: alphabet_t

   The type of the generator set of the automaton.

.. type:: weightset_t

   The type of the weight set of the automaton.

.. type:: kind_t

   The kind of the automaton.

.. type:: entryset_t

   The type of the entry set of the automaton, i.e., a polynomial set
   whose elements can be used to represent the entries of the
   automaton: ``polynomial<alphabet_t,weightset_t>``.

.. type:: state_t

   The type for the states of this automaton.

.. type:: transition_t

   The type for the transitions of this automaton.

.. type:: label_t

   The type use to label the automaton.  This usually depends on :type:`kind_t`.  For
   ``labels_are_letters``, the transitions are labeled by ``alphabet_t::letter_t``,
   while for ``labels_are_words`` they are labeled by ``alphabet_t::word_t``.

.. type:: weight_t

   The type used to stored weights in this automaton.  Equal to ``weightset_t::weight_t``.

.. type:: entry_t

   The type used to represent entry in this automaton.  Equal to ``entryset_t::weight_t``.

Related sets
~~~~~~~~~~~~
.. function:: const alphabet_t&  alphabet() const

   Return the generator set used by this automaton.

.. function:: const weightset_t& weightset() const

   Return the weight set used by this automaton.

.. function:: const entryset_t&  entryset() const

   Return the entry set used by this automaton.

Special states and transitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: state_t pre() const

   Return the pre-initial state.

.. function:: state_t post() const

   Return the post-initial state.

.. function:: state_t invalid_state() const

   Return a value that is an invalid state.  Such a state may for
   instance be used to initialize a :type:`state_t` variable at the start
   of an algorithm.

.. function:: transition_t invalid_transition() const

   Return a value that is an invalid transition.  Such a transition is
   for instance returned by :func:`get_transition` when no
   matching transition is found.


Statistics
~~~~~~~~~~
.. function:: size_t nb_states() const

   Number of states in the automaton.  This does not account for the
   :func:`pre` and :func:`post` states.

.. function:: size_t nb_initials() const

   Number of states that are initial in the automaton.  This is also the number of outgoing
   transitions of :func:`pre`.

.. function:: size_t nb_finals() const

   Number of states that are final in the automaton.  This is also the number of incoming
   transitions of :func:`post`.

.. function:: size_t nb_transitions() const

   Number of transitions of the automaton.  This does not include
   initial transitions (leaving :func:`pre`), and final
   transitions (leaving :func:`post`).

Queries on states
~~~~~~~~~~~~~~~~~

.. function:: bool has_state(state_t s) const

   Whether the automaton has a valid state corresponding to *s*.

.. function:: bool is_initial(state_t s) const

   Whether the state *s* is initial.  You should probably use
   :func:`get_initial_weight` instead.

.. function:: bool is_final(state_t s) const

   Whether the state *s* is final.  You should probably use
   :func:`get_final_weight` instead.

.. function:: weight_t get_initial_weight(state_t s) const

   Return the initial weight of *s*, i.e., the weight that labels an
   initial transition leaving :func:`pre` and going to *s*.  If such
   transition does not exist, ``weightset().zero()`` is returned.

.. function:: weight_t get_final_weight(state_t s) const

   Return the final weight of *s*, i.e., the weight that labels a
   final transition leaving *s* and going to :func:`pre`.  If such
   transition does not exist, ``weightset().zero()`` is returned.

Queries on transitions
~~~~~~~~~~~~~~~~~~~~~~

.. function:: transition_t get_transition(state_t src, state_t dst, label_t l) const

   Get a transition connecting *src* to *dst* with label *l*.  If no such transition
   exists, return :func:`invalid_transition`.

.. function:: bool has_transition(state_t src, state_t dst, label_t l) const

   Whether the automaton has a transition labeled by *l* between
   states *src* and *dst*.

   This is actually syntactic sugar for::

      return get_transition(src, dst, l) != invalid_transition();

.. function:: bool has_transition(transition_t t) const

   Whether the automaton has a valid transition corresponding to *t*.

.. function:: state_t src_of(transition_t t) const
              state_t dst_of(transition_t t) const
              label_t label_of(transition_t t) const
              weight_t weight_of(transition_t t) const

.. function:: alphabet_t::word_t word_label_of(transition_t t) const

   Return the label for the transition *t* as a word.  For
   ``labels_are_words`` automata, this is strictly equivalent to
   :func:`label_of`, while for ``labels_are_letters`` this
   is equivalent to::

      return alphabet().to_word(label_of(t));


Edition of states
~~~~~~~~~~~~~~~~~
.. function:: state_t new_state()

   Create a new state.

.. function:: void del_state(state_t s)

   Delete the state *s*.

.. function:: void set_initial(state_t s, weight_t k)
              void set_initial(state_t s)

   Set the state *s* to be initial with weight *k*.  If the state *s*
   was already initial, its initial weight is replaced by *k*.  If *k*
   is ``weightset().zero()``, then the state becomes non initial.

   If *k* is omitted, it default to ``weightset().unit()``.

.. function:: weight_t add_initial(state_t s, weight_t k)

   Add the weight *k* to the initial weight of *s* and return the sum.
   It is possible to call this method on a state which was not
   initial, in which case its new initial weight is *k*.  If the
   results equals to ``weightset().zero()``, the state becomes non
   initial.

.. function:: void unset_initial(state_t s)

   Syntactic sugar for::

      set_initial(s, weightset().zero());

.. function:: void set_final(state_t s, weight_t k)

   Set the state *s* to be final with weight *k*.  If the state *s*
   was already final, its final weight is replaced by *k*.  If *k*
   is ``weightset().zero()``, then the state becomes non final.

.. function:: void set_final(state_t s)

   Syntactic sugar for::

      set_final(s, weightset().unit());

.. function:: weight_t add_final(state_t s, weight_t k)

   Add the weight *k* to the final weight of *s* and return the sum.
   It is possible to call this method on a state which was not final,
   in which case its new final weight is *k*.  If the results equals
   to ``weightset().zero()``, the state becomes non initial.

.. function:: void unset_final(state_t s)

   Syntactic sugar for::

      set_final(s, weightset().zero());


Edition of transitions
~~~~~~~~~~~~~~~~~~~~~~

.. function:: void del_transition(transition_t t)

   Remove the transition *t*.

.. function:: void del_transition(state_t src, state_t dst, label_t l)

   Remove any transition from *src* to *dst* with label *l*.  If there is
   no such transition, this method has no effect.

.. function:: transition_t set_transition(state_t src, state_t dst, label_t l, weight_t k)
              transition_t set_transition(state_t src, state_t dst, label_t l)

   Sets a transition between *src* and *dst* with label *l* and weight
   *k*.  If a transition between *src* and *dst* with label *l*
   already exists, its weight is replaced by *k*.  If *k* equals to
   ``weightset().zero()``, the transition is deleted.

   If *k* is omitted, it defaults to ``weightset().unit()``.

   .. Note::

      :func:`pre` can only be used as a source, and
      :func:`post` can only be used as a destination.  Furthermore,
      These two states cannot be connected directly by a transition.

      There is no check performed on the label of such transitions.
      Maybe we want one?

.. function:: weight_t add_transition(state_t src, state_t dst, label_t l, weight_t k)
              weight_t add_transition(state_t src, state_t dst, label_t l)

   Add *k* to the weight of a transition from *src* to *dst* labeled
   by *l* if such a transition exists, or create the transition otherwise.

   If *k* is omitted, it defaults to ``weightset().unit()``.

.. function:: weight_t set_weight(transition_t t, weight_t k)

   Overwrite the weight of transition *t* with *k*.  If *k* equals to
   ``weightset().zero()``, the transition is deleted.

.. function:: weight_t add_weight(transition_t t, weight_t k)

   Add *k* to the current weight of transition *t*.  If the result
   equals to ``weightset().zero()``, the transition is deleted.


Iteration on states and transitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the following, *state_container* and *transition_container* are
fictive types.  These pseudo containers contain the minimum interface
(i.e., ``begin()`` and ``end()`` methods), only guarantee is that they
can be iterated over using a classical STL-loop.

.. function:: state_container states() const
              state_container all_states() const

   All states of the automaton.  The first flavor excludes :func:`pre`
   and :func:`post` states, while the second includes them.

.. function:: transition_container transitions() const
              transition_container all_transitions() const

   All transitions of the automaton.  The first
   flavor excludes initial and final transitions, while
   the second flavor include them.

.. function:: transition_container initials() const

   All initial *transitions*.

   One can iterate over all the initial *states* of an automaton
   ``aut`` and retrieve the associated initial weights using a loop
   such as::

     for (auto t : aut.initials()) {
        auto s = aut.dst_of(t);
        auto k = aut.weight_of(t);
        // use state s and weight k ...
     }

.. function:: transition_container finals() const

   All final *transitions*.

   One can iterate over all the final *states* of an automaton
   ``aut`` and retrieve the associated final weights using a loop
   such as::

     for (auto t : aut.initials()) {
        auto s = aut.src_of(t);
        auto k = aut.weight_of(t);
        // use state s and weight k ...
     }

.. function:: transition_container out(state_t s) const
              transition_container all_out(state_t s) const

   All outgoing transitions of state *s*.  The first flavor
   excludes final transitions, while the second flavor include them.

.. function:: transition_container out(state_t s, const label_t& l) const

   All outgoing transitions of state *s* with label *l*.

.. function:: transition_container in(state_t s) const
              transition_container all_in(state_t s) const

   All incoming transitions of state *s*.  The first flavor
   excludes initial transitions, while the second flavor include them.

.. function:: transition_container in(state_t s, const label_t& l) const

   All incoming transitions of state *s* with label *l*.

.. function:: transition_container outin(state_t s, state_t d) const

   All transitions between states *s* and *d*.

Iteration on entries
~~~~~~~~~~~~~~~~~~~~

.. function:: entry_t entry_at(state_t s, state_t d) const

   Return the entry :math:`(s,d)`, that is, the polynomial
   representing all transitions between *s* and *d*.

.. function:: entry_t entry_at(transition_t t) const

   Syntactic sugar for::

      return entry_at(src_of(t), dst_of(t));

.. function:: transition_container entries() const
              transition_container all_entries() const

   A container that will iterate over all pairs of states that are
   connected in the automaton.  The first flavor excludes pairs the
   contain :func:`pre` or :func:`post`, while the second flavor includes
   them.

   For each pair, a random transition is selected, so that
   :func:`entry_at` can by used to compute the entry between this pair
   of states.

   One way to iterate over all entries of an automaton `aut` is as
   follows::

     for (auto t : aut.entries()) {
        auto src = aut.src_of(t);
        auto ent = aut.entry_at(t);
	auto dst = aut.dst_of(t);
	// ...
     }

.. todo:: Functions for setting entries are missing.

Available Automata
------------------

.. class:: mutable_automaton<Alphabet, WeightSet, Kind>

   An automaton on the alphabet *Alphabet* with weights in *WeightSet*
   and kind *Kind*, implementing all the above interface,

   Defined in ``vcsn/core/mutable_automaton.hh``.

   .. function:: mutable_automaton(const Alphabet& a, const WeightSet& ws)
                 mutable_automaton(const Alphabet& a)

     The constructor for a mutable automaton takes an instance of a
     generator set *a* and an instance of the weight set *ws*.  The
     latter can be omitted if ``WeightSet`` has a default constructor.
