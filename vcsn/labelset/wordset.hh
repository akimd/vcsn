#ifndef VCSN_LABELSET_WORDSET_HH
# define VCSN_LABELSET_WORDSET_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/misc/stream.hh> // conv.

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are words.
    template <typename GenSet>
    class wordset: public genset_labelset<GenSet>
    {
    public:
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using value_t = word_t;
      using label_t = value_t;

      using kind_t = labels_are_words;

      wordset(const genset_ptr& gs)
        : super_type{gs}
      {}

      wordset(const genset_t& gs = {})
        : wordset{std::make_shared<const genset_t>(gs)}
      {}

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "law_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "law_" + super().vname(full);
      }

      /// Build from the description in \a is.
      static wordset make(std::istream& is)
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

      label_t
      special() const
      {
        return this->genset()->template special<label_t>();
      }

      bool
      is_special(const label_t& v) const
      {
        return v == special();
      }

      bool
      is_valid(const label_t& v) const
      {
        for (auto l: v)
          if (!this->has(l))
            return false;
        return true;
      }

      label_t
      one() const
      {
        return this->genset()->empty_word();
      }

      bool
      is_one(const label_t& l) const
      {
        return this->genset()->is_empty_word(l);
      }

      // FIXME: Why do I need to repeat this?
      // It should be inherited from genset-labelset.
      label_t
      conv(std::istream& i) const
      {
        return this->genset()->conv(i);
      }

      // FIXME: remove, see todo.txt:scanners.
      label_t
      conv(const std::string& s) const
      {
        return ::vcsn::conv(*this, s);
      }

      std::ostream&
      print(std::ostream& o, const label_t& l) const
      {
        if (is_one(l))
          o << "\\e";
        else if (!is_special(l))
          o << str_escape(l);
        return o;
      }

      std::string
      format(const label_t& l) const
      {
        std::ostringstream o;
        print(o, l);
        return o.str();
      }

    };

    /// Compute the intersection with another alphabet.
    // FIXME: Factor in genset_labelset?
    template <typename GenSet>
    wordset<GenSet>
    intersect(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
    {
      return {intersect(*lhs.genset(), *rhs.genset())};
    }

    /// Compute the union with another alphabet.
    template <typename GenSet>
    wordset<GenSet>
    get_union(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
    {
      return {get_union(*lhs.genset(), *rhs.genset())};
    }

  }
}

#endif // !VCSN_LABELSET_WORDSET_HH
