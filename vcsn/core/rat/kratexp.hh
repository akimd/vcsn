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

      /// The type of this kratexp.
      virtual type_t type() const = 0;

      /// Whether sum, prod, or star.
      bool is_inner() const
      {
        type_t t = type();
        return t == SUM || t == PROD || t == STAR;
      }
    };


    /*----------.
    | kratexp.  |
    `----------*/

    template <typename Atom, typename Weight>
    class kratexp : public exp
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using kratexp_t = rat::kratexp<atom_value_t, weight_t>;
      using self_t = kratexp;
      using value_t = std::shared_ptr<const kratexp_t>;
      /// Same as value_t, but writable.  Use with care.
      using wvalue_t = std::shared_ptr<kratexp_t>;
      using kratexps_t = std::vector<value_t>;
      using const_visitor = vcsn::rat::const_visitor<atom_value_t, weight_t>;

      kratexp(const weight_t& l);
      kratexp(const kratexp& that)
        : lw_(that.lw_)
      {}

      using shared_t = std::shared_ptr<const kratexp>;
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

    template <typename Atom, typename Weight>
    class inner : public kratexp<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = kratexp<atom_value_t, weight_t>;
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

    template <typename Atom, typename Weight>
    class nary: public inner<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = inner<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
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

    template <typename Atom, typename Weight>
    class prod : public nary<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = nary<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using kratexps_t = typename kratexp_t::kratexps_t;
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

      virtual type_t type() const { return kratexp_t::PROD; };

      virtual void accept(typename kratexp_t::const_visitor& v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    /*------.
    | sum.  |
    `------*/

    template <typename Atom, typename Weight>
    class sum : public nary<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = nary<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using kratexps_t = typename kratexp_t::kratexps_t;
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

      virtual type_t type() const { return kratexp_t::SUM; };

      virtual void accept(typename kratexp_t::const_visitor& v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

    /*-------.
    | star.  |
    `-------*/

    template <typename Atom, typename Weight>
    class star : public inner<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = inner<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using self_t = star;

      star(const weight_t& l, const weight_t& r, value_t exp);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return kratexp_t::STAR; };

      const value_t sub() const;

      virtual void accept(typename kratexp_t::const_visitor &v) const;

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

    template <typename Atom, typename Weight>
    class leaf : public kratexp<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using super_type = kratexp_t;
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


    template <typename Atom, typename Weight>
    class one : public leaf<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = leaf<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using self_t = one;

      one(const weight_t& l);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return kratexp_t::ONE; };

      virtual void accept(typename kratexp_t::const_visitor &v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

    template <typename Atom, typename Weight>
    class zero : public leaf<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = leaf<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using self_t = zero;

      zero(const weight_t& l);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return kratexp_t::ZERO; };

      virtual void accept(typename kratexp_t::const_visitor &v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    template <typename Atom, typename Weight>
    class atom : public leaf<Atom, Weight>
    {
    public:
      using atom_value_t = Atom;
      using weight_t = Weight;
      using super_type = leaf<atom_value_t, weight_t>;
      using kratexp_t = kratexp<atom_value_t, weight_t>;
      using type_t = typename kratexp_t::type_t;
      using value_t = typename kratexp_t::value_t;
      using self_t = atom;

      atom(const weight_t& l, const atom_value_t& value);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const
      {
        return std::static_pointer_cast<const self_t>(clone_());
      };

      virtual type_t type() const { return kratexp_t::ATOM; };

      virtual void accept(typename kratexp_t::const_visitor &v) const;
      const atom_value_t& value() const;

    private:
      atom_value_t value_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/kratexp.hxx>

#endif // !VCSN_CORE_RAT_KRATEXP_HH
