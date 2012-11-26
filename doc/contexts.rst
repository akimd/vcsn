Contexts
========

A context is essentially a triplet that groups a :doc:`generator
set<gensets>` with a :doc:`weight set<weightsets>` and a
:doc:`kind<kinds>`.

Overview
--------

A context offers the following interface::

    template <typename GenSet, typename WeightSet, typename Kind>
    class context: public dyn::context
    {
    public:
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using genset_ptr = std::shared_ptr<const genset_t>;
      using weightset_ptr = std::shared_ptr<const weightset_t>;
      using kind_t = Kind;
      enum
        {
          is_lae = std::is_same<kind_t, labels_are_empty>::value,
          is_lal = std::is_same<kind_t, labels_are_letters>::value,
          is_law = std::is_same<kind_t, labels_are_words>::value,
        };

      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename label_trait<kind_t, genset_t>::label_t;
      using word_t = typename genset_t::word_t;
      /// Type of weights.
      using weight_t = typename weightset_t::value_t;
      /// Type of RatExp kratexps objects.
      using node_t = rat::node<label_t, weight_t>;
      using kratexp_t = std::shared_ptr<const node_t>;
      using kratexpset_t = typename vcsn::kratexpset<context>;
      /// Type of RatExp visitor.
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      context(const context& that);
      context(const genset_ptr& gs, const weightset_ptr& ws);
      context(const genset_t& gs = {}, const weightset_t& ws = {});

      static std::string sname();
      virtual std::string vname() const override final;
      virtual std::string genset_string() const override final;
      const genset_ptr& genset() const;
      const weightset_ptr& weightset() const;

      kratexpset_t make_kratexpset() const;

      std::string format(const kratexp_t& e) const;
      kratexp_t downcast(const rat::exp_t& e) const;
    };

.. todo:: Document the context interface and the available contexts.
