.. default-domain:: cpp

Weight-Sets
===========

.. index:: storage type, weight-set

A weight in Vaucanson2 has two types: a *storage type* and a *semantic
type*.

The :dfn:`storage type` (such as ``bool``, ``int``, ``float``, or more
complex types) is mainly an implementation issue: it specifies how
weights are stored on transitions and passed around between functions.

The storage type does **not** indicate how the weight should be
interpreted.  For instance an integer stored as an ``int`` could be
interpreted as an element of the semiring
:math:`(\mathbb{Z},+,\times,0,1)` or as an element of the semiring
:math:`(\mathbb{Z}\cup\{\infty\},\min,+,\infty,0)`.

A :dfn:`weight-set` object stores the semantic information associated
to the weights.  Ideally you should have one weight-set instantiated
somewhere in order to manipulate weights.

Overview
--------

Here is the required interface of a *Weight-Set* object::

  static std::string sname();

  typedef ... value_t;

  value_t add(const value_t l, const value_t r) const;
  value_t mul(const value_t l, const value_t r) const;

  value_t zero() const;
  value_t unit() const;

  bool is_zero(const value_t v) const;
  bool is_unit(const value_t v) const;

  static constexpr bool show_unit();
  static constexpr bool is_positive_semiring();

  value_t conv(std::string& str) const;

  std::string format(const value_t v) const;
  std::ostream& print(std::ostream& o, const value_t v) const;

Implementations of weight-sets with a complex storage type may decide
to receive them as `const value_t&` and emit them as `const value_t&`
instead of the above pass-by-copy specifications.

Detailed interface
------------------

.. function:: static std::string sname()

   The (static) name of the weight-set (i.e., it's class name).

.. type:: value_t

   The storage type of the weight-set's elements.

.. function:: value_t add(const value_t l, const value_t r) const

   Add two weights and return a new one.

.. function:: value_t mul(const value_t l, const value_t r) const

   Multiply two weights and return a new one.

.. function:: value_t star(const value_t v) const

   Return the star of the value ``v``.
   Raises ``std::domain_error`` if ``v`` is not starable.

   .. todo:: We do not have a ``is_starable`` method.

.. function:: value_t zero() const

   Return the weight that is the identity element for the addition.

.. function:: value_t unit() const

   Return the weight that is the identity element for the multiplication.

.. function:: bool is_zero(const value_t v) const

   Whether ``v == zero()``.

.. function:: bool is_unit(const value_t v) const

   Whether ``v == unit()``.

.. function:: static constexpr bool show_unit()

   Whether it is customary to show the unit weight for this weight
   set.  For instance in a rational expression with weights in
   :math:`(\mathbb{Z},+,\times,0,1)` we prefer ``a + b`` to the more
   explicit ``{1}a + {1}b``, however in
   :math:`(\mathbb{Z}\cup\{\infty\},\min,+,\infty,0)` we want to display ``{0}a +
   {0}b`` even though ``0`` is the unit element, so there is no risk
   to confuse it with ``{1}a + {1}b``.

.. function:: static constexpr bool is_positive_semiring()

   .. index:: positive semiring

   Whether this weight-set is a *positive semiring*.

   :math:`(\mathbb{K},+,\times,0,1)` is a :dfn:`positive semiring` if
   it is *zero-divisor-free* and *zero-sum-free*: :math:`\forall
   k,\ell\in\mathbb{K}\setminus\{0\}, k\times\ell \ne 0` and
   :math:`k + \ell \ne 0`.

   For instance :math:`(\mathbb{Z}\cup\{\infty\},\min,+,\infty,0)` is a positive
   semiring, but :math:`(\mathbb{Z},+,\times,0,1)` is not.

.. function:: value_t transpose(const value_t v) const

   Return the transpose of the value ``v``.

.. function:: value_t conv(std::string& str) const

   Convert a string ``str`` into a weight.  A ``std::domain_error``
   exception is raised if the string cannot be parsed.

.. function:: std::string format(const value_t v) const

   Format a weight ``v`` as a string.

.. function:: std::ostream& print(std::ostream& o, const value_t v) const

   Print a weight ``v`` on the output stream ``o`` and return ``o``.

   It is usually more efficient to call ``print(std::cout, v)``
   instead of ``std::cout << format(v)``, because no intermediate
   string is created.

Available Weight-Sets
---------------------

.. toctree::
   :maxdepth: 2

   scalars
   poly
   kratexp
