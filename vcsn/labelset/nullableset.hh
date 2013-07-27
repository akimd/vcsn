#ifndef VCSN_LABELSET_NULLABLESET_HH
# define VCSN_LABELSET_NULLABLESET_HH

# include <memory>
# include <set>
# include <sstream>

# include <vcsn/alphabets/setalpha.hh> // intersect
# include <vcsn/core/kind.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/stream.hh> // conv.
# include <vcsn/labelset/genset-labelset.hh>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are nullables (letter or empty).
    template <typename GenSet>
    struct nullableset: genset_labelset<GenSet>
    {
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using label_t = typename genset_t::letter_t;
      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_nullable;

      nullableset(const genset_ptr& gs)
        : super_type{gs}
      {}

      nullableset(const genset_t& gs = {})
        : nullableset{std::make_shared<const genset_t>(gs)}
      {}

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "lan_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "lan_" + super().vname(full);
      }

      /// Build from the description in \a is.
      static nullableset make(std::istream& is)
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
        return {gs};
      }

      constexpr label_t
      special() const
      {
        return this->genset()->template special<label_t>();
      }

      /// Whether \a l is the special label (for pre/post transitions).
      bool
      is_special(label_t l) const
      {
        return l == special();
      }

      constexpr label_t
      one() const
      {
        return this->genset()->one_letter();
      }

      bool
      is_one(label_t l) const
      {
        return l == one();
      }

      bool
      is_valid(label_t v) const
      {
        return this->has(v) || is_one(v);
      }

      /// \throws std::domain_error if there is no label here.
      label_t
      conv(std::istream& i) const
      {
        label_t res;
        int c = i.peek();
        if (c == '\\')
          {
            i.ignore();
            c = i.peek();
            if (c != 'e')
              throw std::domain_error("invalid label: unexpected \\"
                                      + str_escape(c));
            i.ignore();
            res = one();
          }
        else if (this->has(c))
          {
            res = c;
            i.ignore();
          }
        else
          throw std::domain_error("invalid label: unexpected "
                                  + str_escape(c));
        return res;
      }

      // FIXME: remove, see todo.txt:scanners.
      label_t
      conv(const std::string& s) const
      {
        return ::vcsn::conv(*this, s);
      }

      std::ostream&
      print(std::ostream& o, label_t l) const
      {
        if (is_one(l))
          o << "\\e";
        else if (!is_special(l))
          o << l;
        return o;
      }

      std::string
      format(label_t v) const
      {
        std::ostringstream o;
        print(o, v);
        return o.str();
      }
    };

    /// Compute the intersection with another alphabet.
    template <typename GenSet>
    nullableset<GenSet>
    intersect(const nullableset<GenSet>& lhs, const nullableset<GenSet>& rhs)
    {
      return {intersect(*lhs.genset(), *rhs.genset())};
    }

  }
}

#endif // !VCSN_LABELSET_NULLABLESET_HH
