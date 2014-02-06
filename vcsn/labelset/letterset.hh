#ifndef VCSN_LABELSET_LETTERSET_HH
# define VCSN_LABELSET_LETTERSET_HH

# include <memory>

# include <vcsn/alphabets/setalpha.hh> // intersection
# include <vcsn/core/kind.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/set.hh> // intersection
# include <vcsn/labelset/genset-labelset.hh>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are letters.
    template <typename GenSet>
    struct letterset: genset_labelset<GenSet>
    {
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using value_t = letter_t;

      using kind_t = labels_are_letters;

      letterset(const genset_ptr& gs)
        : super_type{gs}
      {}

      letterset(const genset_t& gs = {})
        : letterset{std::make_shared<const genset_t>(gs)}
      {}

      letterset(const std::initializer_list<letter_t>& letters)
        : letterset{std::make_shared<const genset_t>(letters)}
      {}

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "lal_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "lal_" + super().vname(full);
      }

      /// Build from the description in \a is.
      static letterset make(std::istream& is)
      {
        // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
        //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
        //        |   |    |        weightset
        //        |   |    +-- gens
        //        |   +-- letter_type
        //        +-- kind
        kind_t::make(is);
        eat(is, '_');
        auto gs = genset_t::make(is);
        return gs;
      }

      value_t
      special() const ATTRIBUTE_PURE
      {
        return this->genset()->template special<value_t>();
      }

      /// Whether \a l == \a r.
      static bool
      equals(const value_t l, const value_t r)
      {
        return l == r;
      }

      /// Whether \a l < \a r.
      static bool less_than(const value_t l, const value_t r)
      {
        return l < r;
      }

      bool
      is_special(value_t v) const ATTRIBUTE_PURE
      {
        return v == special();
      }

      static constexpr bool
      is_one(value_t)
      {
        return false;
      }

      bool
      is_valid(value_t v) const
      {
        return this->has(v);
      }

      static size_t size(value_t)
      {
        return 1;
      }

      static size_t hash(value_t v)
      {
        return std::hash_value(v);
      }

      /// Read one letter from i, return the corresponding label.
      value_t
      conv(std::istream& i) const
      {
        int c = i.peek();
        if (this->has(c))
          return i.get();
        else
          throw std::domain_error("invalid label: unexpected "
                                  + str_escape(c));
      }

      std::ostream&
      print(std::ostream& o, const value_t& l,
          const std::string& = "text") const
      {
        if (!is_special(l))
          o << str_escape(l);
        return o;
      }
    };

    /// Compute the meet with another alphabet.
    template <typename GenSet>
    letterset<GenSet>
    meet(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
    {
      return {intersection(*lhs.genset(), *rhs.genset())};
    }

    /// Compute the union with another alphabet.
    template <typename GenSet>
    letterset<GenSet>
    join(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
    {
      return {get_union(*lhs.genset(), *rhs.genset())};
    }
  }
}

#endif // !VCSN_LABELSET_LETTERSET_HH
