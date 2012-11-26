.. default-domain:: cpp

Kinds
-----

Vaucanson distinguishes different *kinds* of automata and rational expressions.

The following types are declared in ``vcsn/core/kind.hh`` and used as
parameters in :doc:`contexts` that are in turn passed to complex types
such as :doc:`automata` or :doc:`kratexp`.

.. class:: labels_are_empty
	   labels_are_letters
	   labels_are_words

   For automata, these kinds are used to select what kind of labels
   should be used by the transitions.  For rational expressions,
   ``labels_are_empty`` is not used, and the other two types indicate
   whether the atoms of an expression are words or letters.

   These types all implement the following static methods.

   .. function:: static std::string sname()

   The (static) name of the kind as a three-letter acronym (i.e.,
   ``"lae"``, ``"lal"``, or ``"law"``).

.. class:: label_traits<Kind, GenSet>

   This traits ease the selection of the type used to store the
   labels, for a given :doc:`generator set<gensets>`.

   .. type:: label_t

      The type used to store the labels for the pair ``Kind`` and
      ``GenSet``.  For instance if Kind is ``labels_are_letters``,
      this returns ``GenSet::letter_t``.


   
