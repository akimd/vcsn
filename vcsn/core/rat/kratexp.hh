#ifndef VCSN_CORE_RAT_KRATEXP_HH
# define VCSN_CORE_RAT_KRATEXP_HH

# include <vector>
# include <string>

# include <boost/range.hpp>

# include <vcsn/core/kind.hh>
# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    class exp
    {
    public:
      virtual ~exp() = 0;

      /// The possible types of kratexps.
      enum type_t
        {
          ZERO = 0,
          ONE  = 1,
          ATOM = 2,
          SUM  = 3,
          PROD = 4,
          STAR = 5,
        };

      /// The type of this node.
      virtual type_t type() const = 0;

      /// Whether sum, prod, or star.
      bool is_inner() const
      {
        type_t t = type();
        return t == SUM || t == PROD || t == STAR;
      }
    };


    /*-------.
    | node.  |
    `-------*/

    template <typename Label, typename Weight>
    class node : public exp
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using node_t = rat::node<label_t, weight_t>;
      using self_t = node;
      using value_t = std::shared_ptr<const node_t>;
      /// Same as value_t, but writable.  Use with care.
      using wvalue_t = std::shared_ptr<node_t>;
      using kratexps_t = std::vector<value_t>;
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      node(const weight_t& l);
      node(const node& that)
        : lw_(that.lw_)
      {}

      using shared_t = std::shared_ptr<const node>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual void accept(const_visitor &v) const = 0;

      const weight_t &left_weight() const;
      weight_t &left_weight();

    protected:
      virtual value_t clone_() const = 0;
      weight_t lw_;
    };


    /*--------.
    | inner.  |
    `--------*/

    template <typename Label, typename Weight>
    class inner : public node<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = node<label_t, weight_t>;
      using value_t = typename super_type::value_t;
      using self_t = inner;

      const weight_t &right_weight() const;
      weight_t &right_weight();

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

    protected:
      inner(const weight_t& l, const weight_t& r);
      inner(const inner& that)
        : super_type(that)
        , rw_(that.rw_)
      {}

      virtual value_t clone_() const = 0;

      weight_t rw_;
    };


    /*-------.
    | nary.  |
    `-------*/

    template <typename Label, typename Weight>
    class nary: public inner<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = inner<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename super_type::value_t;
      using kratexps_t = typename super_type::kratexps_t;
      using self_t = nary;

      using const_iterator = typename kratexps_t::const_iterator;
      using iterator = typename kratexps_t::iterator;
      using const_reverse_iterator = typename kratexps_t::const_reverse_iterator;
      using reverse_iterator = typename kratexps_t::reverse_iterator;

      const_iterator begin() const;
      const_iterator end() const;
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      size_t size() const;

      /// The first item of this nary.
      const value_t head() const { return *begin(); }

      /// The non-first items.
      auto tail() const -> decltype(boost::make_iterator_range(*this, 1, 0))
      {
        return boost::make_iterator_range(*this, 1, 0);
      }

    protected:
      nary(const weight_t& l, const weight_t& r, const kratexps_t& ns = kratexps_t());
      nary(const nary& that)
        : super_type(that)
        , sub_kratexp_(that.sub_kratexp_)
      {}

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };
      virtual value_t clone_() const = 0;

    private:
      kratexps_t sub_kratexp_;
    };


    /*-------.
    | prod.  |
    `-------*/

    template <typename Label, typename Weight>
    class prod : public nary<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = nary<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using kratexps_t = typename node_t::kratexps_t;
      using self_t = prod;

      using const_iterator = typename kratexps_t::const_iterator;
      using iterator = typename kratexps_t::iterator;
      using const_reverse_iterator = typename kratexps_t::const_reverse_iterator;
      using reverse_iterator = typename kratexps_t::reverse_iterator;

      prod(const weight_t& l, const weight_t& r, const kratexps_t& ns = kratexps_t());

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::PROD; };

      virtual void accept(typename node_t::const_visitor& v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    /*------.
    | sum.  |
    `------*/

    template <typename Label, typename Weight>
    class sum : public nary<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = nary<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using kratexps_t = typename node_t::kratexps_t;
      using self_t = sum;

      using const_iterator = typename kratexps_t::const_iterator;
      using iterator = typename kratexps_t::iterator;
      using const_reverse_iterator = typename kratexps_t::const_reverse_iterator;
      using reverse_iterator = typename kratexps_t::reverse_iterator;

      sum(const weight_t& l, const weight_t& r, const kratexps_t& ns = kratexps_t());

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::SUM; };

      virtual void accept(typename node_t::const_visitor& v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

    /*-------.
    | star.  |
    `-------*/

    template <typename Label, typename Weight>
    class star : public inner<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = inner<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using self_t = star;

      star(const weight_t& l, const weight_t& r, value_t exp);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::STAR; };

      const value_t sub() const;

      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      value_t sub_exp_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    /*-------.
    | leaf.  |
    `-------*/

    template <typename Label, typename Weight>
    class leaf : public node<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using super_type = node_t;
      using self_t = leaf;
    protected:
      leaf(const weight_t& l);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };
      virtual value_t clone_() const = 0;
    };


    template <typename Label, typename Weight>
    class one : public leaf<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = leaf<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using self_t = one;

      one(const weight_t& l);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::ONE; };

      virtual void accept(typename node_t::const_visitor &v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

    template <typename Label, typename Weight>
    class zero : public leaf<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = leaf<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using self_t = zero;

      zero(const weight_t& l);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::const_visitor &v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    template <typename Label, typename Weight>
    class atom : public leaf<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = leaf<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using self_t = atom;

      atom(const weight_t& l, const label_t& value);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return node_t::ATOM; };

      virtual void accept(typename node_t::const_visitor &v) const;
      const label_t& value() const;

    private:
      label_t value_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/kratexp.hxx>

#endif // !VCSN_CORE_RAT_KRATEXP_HH
