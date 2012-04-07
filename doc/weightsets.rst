.. default-domain:: cpp

Weight Sets
===========

A weight in Vaucanson2 has two types: a *storage type* and a *semantic
type*.

The storage type (such as ``int``, ``bool``, ``float``, or more
complex types) is mainly an implementation issue: it specifies how
weights are stored on transitions and passed around between functions.

The storage type does **not** indicate how the weight should be
interpreted.  For instance an integer stored by ``int`` could be
interpreted as an element of the semiring
:math:`(\mathbb{Z},+,\times,0,1)` or as an element of the semiring
:math:`(\mathbb{Z},\min,+,\infty,0)`.

*Weight Sets* objects store the semantic information associated to the
weights.  Ideally you should have one weight set instantiated
somewhere in order to manipulate weights.

Overview
--------

Here is the required interface of a *Weight Set* object::

    typedef ... value_t;

    value_t add(const value_t l, const value_t r) const;
    value_t mul(const value_t l, const value_t r) const;

    value_t zero() const;
    value_t unit() const;

    bool is_zero(const value_t v) const;
    bool is_unit(const value_t v) const;

    bool show_unit() const;

    value_t conv(std::string& str) const;

    std::string format(const value_t v) const;
    std::ostream& print(std::ostream& o, const value_t v) const;

Implementations of weight sets with a complex storage type may decide
to receive them as `const value_t&` and emit them as `const value_t&`
instead of the above pass-by-copy specifications.

Detailed interface
------------------

.. type:: value_t

   The storage type of the weight set's elements.

.. function:: value_t add(const value_t l, const value_t r) const

   Add two weights and return a new one.

.. function:: value_t add(const value_t l, const value_t r) const

   Multiply two weights and return a new one.

.. function:: value_t zero() const

   Return the weight that is neutral for the addition.

.. function:: value_t unit() const

   Return the weight that is neutral for the multiplication.

.. function:: bool is_zero(const value_t v) const

   Whether ``v == zero()``.

.. function:: bool is_unit(const value_t v) const

   Whether ``v == unit()``.

.. function:: bool show_unit() const

   Whether it is customary to show the unit weight for this weight
   set.  For instance in a rational expression with weights in
   :math:`(\mathbb{Z},+,\times,0,1)` we prefer ``a + b`` to the more
   explicit ``{1}a + {1}b``, however in
   :math:`(\mathbb{Z},\min,+,\infty,0)` we want to display ``{oo}a +
   {oo}b`` even though ``oo`` is the unit element, so there is no risk
   to confuse it with ``{1}a + {1}b``.

.. function:: value_t conv(std::string& str) const

   Convert a string ``str`` into a weight.  A ``std::domain_error``
   exception is raised of the string cannot be parsed.

.. function:: std::string format(const value_t v) const

   Format a weight ``v`` as a string.

.. function:: std::ostream& print(std::ostream& o, const value_t v) const

   Print a weight ``v`` on the output stream ``o`` and return ``o``.

   This is usually more efficient to call ``print(std::cout, v)``
   instead of ``std::cout << format(v)``, because no intermediate
   string is created.

Available Weight Sets
---------------------

The following weight sets are implemented:

.. class:: b

   The classical Boolean semiring :math:`(\mathbb{B},\lor,\land,0,1)`,
   with elements stored as ``bool``.

   Defined in ``vcsn/weights/b.hh``.

.. class:: z

   The usual integer semiring :math:`(\mathbb{Z},+,\times,0,1)`,
   with elements stored as ``int``.

   Defined in ``vcsn/weights/z.hh``.

.. class:: z_min

   The usual integer semiring :math:`(\mathbb{Z},\min,+,\infty,0)`,
   with elements stored as ``int``.

   Defined in ``vcsn/weights/z_min.hh``.

.. class:: polynomial<A, W>

   Polynomials with letters in the alphabet ``A`` and weights in ``W``.

   Defined in ``vcsn/weights/poly.hh``.

.. class:: factory_<A, W>

   Rational expression over the alphabet ``A`` with weight in ``W``.

   Defined in ``vcsn/core/rat/factory_.hh``.

.. todo:: Polynomial and rational expressions have to be documented
          separately.
