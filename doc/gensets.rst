.. default-domain:: cpp

Generators Sets
===============

Generators sets offers functions to build words over a set of
*generators* (called letters).


Overview
--------

Generators sets offer the following interface::

    static std::string sname();

    typedef ... letter_t;
    typedef ... word_t;
    typedef ::vcsn::empty_t empty_t;

    typedef ... iterator_t;

    bool has(typename letter_t l) const;

    iterator_t begin() const;
    iterator_t end() const;

    word_t to_word(const empty_t) const;
    word_t to_word(const letter_t l) const;
    word_t to_word(const word_t& l) const;

    word_t concat(const letter_t l, const letter_t r);
    word_t concat(const word_t l, const letter_t r) const;
    word_t concat(const letter_t l, const word_t r) const;
    word_t concat(const word_t l, const word_t r) const;

    word_t identity() const;
    bool is_identity(const word_t& w) const;

    word_t transpose(const word_t& w) const;
    letter_t transpose(letter_t l) const;
    empty_t transpose(empty_t) const;

    bool equals(empty_t, empty_t) const;
    bool equals(const letter_t& l1, const letter_t& l2) const;
    bool equals(const word_t& w1, const word_t& w2) const;

    bool is_letter(const empty_t&) const;  
    bool is_letter(const letter_t&) const;
    bool is_letter(const word_t& w) const;

    std::ostream& print(std::ostream& o, const empty_t&) const;  // REMOVE?
    std::ostream& print(std::ostream& o, const letter_t& l) const;
    std::ostream& print(std::ostream& o, const word_t& w) const;

    std::string format(const letter_t l) const;
    std::string format(const word_t& w) const;

    template<class T = letter_t>
    T special() const;


Detailed interface
------------------

.. function:: static std::string sname()

   The (static) name of the weight set (i.e., its class name).

.. type:: letter_t

   The type used to represent letters.   

.. type:: word_t

   The type used to represent words built from ``letter_t`` elements.   

.. type:: empty_t

   The type use to represent an empty word (in LAE) context.
   
   .. todo:: the use of ``empty_t`` in this whole interface is quite
             unsatisfactory.  Maybe we should implement
	     an *empty generator set* for LAR contexts.

.. type:: iterator_t

   The type of an iterator over the generators.

.. function:: bool has(typename letter_t l) const

   Whether ``l`` is a generator.  

.. function:: iterator_t begin() const
	      iterator_t end() const
   
   Iterators over the generators.

.. function:: word_t to_word(const empty_t) const
	      word_t to_word(const letter_t l) const
	      word_t to_word(const word_t& l) const

   Conversion to words.

.. function:: word_t concat(const letter_t l, const letter_t r)
	      word_t concat(const word_t l, const letter_t r) const
	      word_t concat(const letter_t l, const word_t r) const
	      word_t concat(const word_t l, const word_t r) const

   Concatenation of two letters or words to form a new word.

.. function:: word_t identity() const

   Returns the empty word.

.. function:: bool is_identity(const word_t& w) const

   Test whether a word is empty.

.. function:: word_t transpose(const word_t& w) const
	      letter_t transpose(letter_t l) const
	      empty_t transpose(empty_t) const

   Transpose a word.  Calling transpose on letters or ``empty_t`` has
   no effect.

.. function:: bool equals(empty_t, empty_t) const
	      bool equals(const letter_t& l1, const letter_t& l2) const
	      bool equals(const word_t& w1, const word_t& w2) const

   Test equality between two words or letters.

.. function:: bool is_letter(const empty_t&) const
	      bool is_letter(const letter_t&) const
	      bool is_letter(const word_t& w) const

    Check whether a word is a letter (i.e., has size 1).

.. function:: std::ostream& print(std::ostream& o, const empty_t&) const
	      std::ostream& print(std::ostream& o, const letter_t& l) const
	      std::ostream& print(std::ostream& o, const word_t& w) const

    Print a word ``w`` or letter ``l`` on a stream ``o``. 
    
    .. todo:: The current implementation for ``empty_t`` print
	      ``"EMPTY"``.  It would probably makes more sense to
	      print ``""`` or ``"\\e"``.  The current situation seems
	      to indicate that the function is never used, yet
	      removing it breaks the build.

.. function:: std::string format(const letter_t l) const
	      std::string format(const word_t& w) const
    
    Format a word ``w`` or letter ``l`` as a string.  It is more
    efficient to call ``print()`` if that string is to be printed
    right away.

.. function:: T special() const

   Return the special letter to be used to label transitions leaving
   :func:`pre` or reaching :func:`post` in :doc:`automata`.


Implementations
---------------

There is currently only one implementation of the above interface, the
``set_alphabet<char_letter>`` class.  Letters are stored as ``char``,
words as ``std::string``, and the ``set_alphabet<char_letter>``
instance keeps an alphabet (a list of allowed letters) as an
``std::set<char>``.

This alphabet can be populated either at construction time, or
using the function ``add_letter``.


.. function:: set_alphabet()
	      set_alphabet(const std::initializer_list<letter_t>& l)

   Instantiate a ``set_alphabet``, either with an empty alphabet, or with an alphabet initialized from a list of letters.

.. function:: set_alphabet& add_letter(typename letter_t l)

   Add ``l`` to the set of letters of the alphabet.
