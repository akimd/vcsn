.. default-domain:: cpp

Polynomials
===========

The :cpp:class:`polynomialset` class handles polynomials whose
variables are letters of an alphabets (possibly combined to form
words), and coefficients are weights.  For instance::

  {3}ab + {4}\e + a

is a polynomial over the alphabet :math:`\{a,b\}`, and the weight set
:cpp:class:`z` (for instance).  It has three monomials: ``{3}ab``,
``{4}\e`` and ``a``.

Polynomials are implemented as an ``std::map`` between words and
weights.  The above example implement the following mapping::

  "" -> 4
  "a" -> 1
  "ab" -> 3

.. class:: polynomialset<C>

   Polynomials over the context ``C`` (i.e., with
   letters in ``C::genset_t`` and weights in ``C::weightset_t``).

   The only polynomials that are starable are the empty polynomial
   (denoted ``"\z"``), and polynomials of the form ``"{k}\e"`` where
   the weight ``k`` is starable.

   Defined in ``vcsn/weights/poly.hh``.

Overview
--------

``polynomialset`` implements the :doc:`Weight Set interface
<weightsets>` (they are more usually used as *entries* rather than
*weights*) and adds a few other methods specific to the handling of
polynomials::

  template <class Context>
  struct polynomialset
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;

    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using word_t = typename genset_t::word_t;
    using weight_t = typename context_t::weight_t;

    using value_t = std::map<word_t, weight_t>;

    polynomialset(const context_t& ctx);

    // Dealing with context
    const context_t& context() const { return ctx_; }
    const genset_ptr& genset() const { return ctx_.genset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

    // Polynomial specific methods
    value_t& del_weight(value_t& v, const word_t& w) const;
    value_t& set_weight(value_t& v, const word_t& w, const weight_t k) const;
    value_t& add_weight(value_t& v, const word_t& w, const weight_t k) const;
    const value_t get_weight(value_t& v, const word_t& w) const;

    // weight set interface
    std::string sname() const;
    value_t add(const value_t& l, const value_t& r) const;
    value_t mul(const value_t& l, const value_t& r) const;
    value_t star(const value_t& v) const;
    const value_t& unit() const;
    bool is_unit(const value_t& v) const;
    const value_t& zero() const;
    bool is_zero(const value_t& v) const;
    static constexpr bool show_unit() { return true; }
    static constexpr bool is_positive_semiring();
    value_t transpose(const value_t v) const;
    value_t conv(const std::string& s) const;
    std::ostream& print(std::ostream& out, const value_t& v) const;
    std::string format(const value_t& v) const;
  };

New methods
-----------

We only describe methods that are not part of the
:doc:`Weight Set interface <weightsets>`.


.. function:: polynomialset<context_t>(const context_t& ctx)

   Construct a polynomial set for context ``ctx``.  Note that
   currently the kind of the context is ignored.

.. function:: const context_t& context() const
	      const genset_ptr& genset() const
	      const weightset_ptr& weightset() const

   Return the context, generator set, or weight set used.

.. function:: value_t& del_weight(value_t& v, const word_t& w)

   Remove the monomial associated to ``w``.

.. function:: value_t& set_weight(value_t& v, const word_t& w, const weight_t k) const

   Associate weight ``k`` to the word ``w`` in polynomial ``v``.
   If the weight is zero, :cpp:func:`del_assoc()` is called.

.. function:: value_t& add_weight(value_t& v, const word_t& w, const weight_t k) const

   Add ``k`` to the weight of ``w`` in ``v``.  If ``w`` did not exist,
   its weight is assumed to be zero.  If the new weight is zero, ``w``
   is removed from ``v``.

.. function:: const weight_t get_weight(value_t& v, const word_t& w) const

   Return the weight associated to word ``w`` in polynomial ``v``.
