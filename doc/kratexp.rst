.. default-domain:: cpp

Weighted Rational Expressions
=============================

.. class:: kratexpset<C>

   Weighted rational expressions over the context ``C``, i.e., with
   weights in ``C::weightset_t``, and atoms (= leaves) of type
   ``C::label_t``.

   Defined in ``vcsn/core/rat/kratexpset.hh``.

Overview
========

``kratexpset`` implements the :doc:`Weight Set interface <weightsets>`
and adds other methods specific to the handling of rational
expressions::

  template <typename Context>
  class kratexpset
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;
    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using label_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;

    // The storage type of a rational expression.
    using value_t = ...;

    kratexpset(const context_t& ctx);
    const context_t& context() const;
    const genset_ptr& genset() const;
    const weightset_ptr& weightset() const;
    value_t atom(const label_t& v) const;
    value_t concat(value_t l, value_t r) const;
    value_t weight(value_t e, const weight_t& w) const;
    value_t weight(const weight_t& w, value_t e) const;

    // Weight Set interface
    static std::string sname();
    bool is_zero(value_t v) const;
    bool is_unit(value_t v) const;
    value_t zero() const;
    value_t unit() const;
    value_t zero(const weight_t& w) const;
    value_t unit(const weight_t& w) const;
    value_t add(value_t l, value_t r) const;
    value_t mul(value_t l, value_t r) const;
    value_t star(value_t e) const;
    value_t transpose(value_t e) const;
    static constexpr bool show_unit();
    static constexpr bool is_positive_semiring();
    value_t conv(const std::string& s) const;
    std::ostream& print(std::ostream& o, const value_t v) const;
    std::string format(const value_t v) const;
  };

New methods
-----------

We only describe methods that are not part of the :doc:`Weight Set
interface <weightsets>`.  The ``add()`` and ``mul()`` function
respectively correspond to choice and concatenation.

.. function:: kratexpset<context_t>(const context_t& ctx)

   Construct a ``kratexpset`` for context ``ctx``.

   ``value_t`` instances created by this class represent weighted
   rational expression using a syntax tree whose internal nodes are
   operators (disjunction, concatenation, Kleen star) and leaves are
   either atoms (of labeled by ``label_t``) or constants (zero or
   one).  The definition of ``label_t`` depends on the *kind* of
   ``ctx``.

   Each internal node has one left weight and one right weight.  (This
   distinction is useful in cases where weight multiplication is not
   commutative.)  Leaves have only one weight.

.. function:: const context_t& context() const
	      const genset_ptr& genset() const
	      const weightset_ptr& weightset() const

   Return the context, generator set, or weight set used.

.. function:: value_t atom(const label_t& v) const

   Create an atom (a leave in the syntax tree) labeled by ``v``.

.. function:: value_t concat(value_t l, value_t r) const

   Implicit concatenation of two expressions.  Use ``mul(l, r)``
   for the regular concatenation operator.

   This function only differs from ``mul()`` only in
   ``label_are_letter`` and ``label_are_empty`` contexts, where
   concatenating ``"(ab).a"`` to ``"b"`` will result in
   ``"(ab).(ab)"`` with implicit concatenation, and ``"(ab).a.b"``
   with the regular concatenation.  This function is probably only
   useful to the parser of rational expressions.

   .. todo:: ``concat()`` should probably be renamed to something like
	     ``iconcat()`` or ``imul()``, with ``i`` standing for
	     *implicit*.

.. function:: value_t weight(value_t e, const weight_t& w) const
              value_t weight(const weight_t& w, value_t e) const

   Multiply the expression ``e`` by ``w``.  The order of the arguments
   determines whether we are using a left or right multiplication.

   Each internal node has one left weight and one right weight.  (This
   distinction is useful in cases where weight multiplication is not
   commutative.)  Leaves have only one weight.

   .. todo:: It could be clearer to have ``lweight()`` and
             ``rweight()``.

