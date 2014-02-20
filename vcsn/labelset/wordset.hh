#ifndef VCSN_LABELSET_WORDSET_HH
# define VCSN_LABELSET_WORDSET_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

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
        return gs;
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
        return (std::forward_as_tuple(l.size(), l)
                < std::forward_as_tuple(r.size(), r));
      }

      value_t
      special() const
      {
        return this->genset()->template special<value_t>();
      }

      bool
      is_special(const value_t& v) const
      {
        return v == special();
      }

      bool
      is_valid(const value_t& v) const
      {
        for (auto l: v)
          if (!this->has(l))
            return false;
        return true;
      }

      static constexpr bool
      has_one()
      {
        return true;
      }

      value_t
      one() const
      {
        return this->genset()->empty_word();
      }

      bool
      is_one(const value_t& l) const ATTRIBUTE_PURE
      {
        return this->genset()->is_empty_word(l);
      }

      static size_t size(const value_t& v)
      {
        return v.length();
      }

      static size_t hash(const value_t& v)
      {
        return hash_value(v);
      }

      // FIXME: Why do I need to repeat this?
      // It should be inherited from genset-labelset.
      value_t
      conv(std::istream& i) const
      {
        return this->genset()->conv(i);
      }

      std::ostream&
      print(std::ostream& o, const value_t& l,
            const std::string& format = "text") const
      {
        if (is_one(l))
          o << (format == "latex" ? "\\varepsilon" : "\\e");
        else if (!is_special(l))
          o << str_escape(l);
        return o;
      }
    };

    /// Compute the meet with another alphabet.
    // FIXME: Factor in genset_labelset?
    template <typename GenSet>
    wordset<GenSet>
    meet(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
    {
      return {intersection(*lhs.genset(), *rhs.genset())};
    }

    /// Compute the union with another alphabet.
    template <typename GenSet>
    wordset<GenSet>
    join(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
    {
      return {get_union(*lhs.genset(), *rhs.genset())};
    }

  }

  template <typename GenSet>
  inline
  std::ostream&
  print_set(const ctx::wordset<GenSet>& ls,
            std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	print_set(*ls.genset(), o, format);
	o << "^*";
      }
    else if (format == "text")
      o << ls.vname(true);
    else
      raise("invalid format: ", format);
    return o;
  }

}

#endif // !VCSN_LABELSET_WORDSET_HH
