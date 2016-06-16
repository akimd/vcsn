#pragma once

#include <memory>

#include <vcsn/alphabets/setalpha.hh> // intersection
#include <vcsn/core/kind.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/set.hh> // intersection
#include <vcsn/labelset/genset-labelset.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/nullableset.hh>
#include <vcsn/labelset/wordset.hh>

namespace vcsn
{
  /// Implementation of labels are letters.
  template <typename GenSet>
  class letterset: public detail::genset_labelset<GenSet>
  {
  public:
    using genset_t = GenSet;
    using super_t = detail::genset_labelset<genset_t>;
    using self_t = letterset;
    using genset_ptr = std::shared_ptr<const genset_t>;

    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using letters_t = std::set<letter_t, vcsn::less<self_t, letter_t>>;

    using value_t = letter_t;
    using values_t = std::set<value_t, vcsn::less<self_t>>;

    using kind_t = labels_are_letters;

    letterset(const genset_ptr& gs)
      : super_t{gs}
    {}

    letterset(const genset_t& gs = {})
      : letterset(std::make_shared<const genset_t>(gs))
    {}

    letterset(std::initializer_list<letter_t> letters)
      : letterset(std::make_shared<const genset_t>(letters))
    {}

    static symbol sname()
    {
      static auto res = symbol{"letterset<" + super_t::sname() + '>'};
      return res;
    }

    /// Build from the description in \a is.
    static letterset make(std::istream& is)
    {
      // name: letterset<char_letters(abc)>.
      //       ^^^^^^^^^ ^^^^^^^^^^^^^^^^^
      //         kind         genset
      eat(is, "letterset<");
      auto gs = genset_t::make(is);
      eat(is, '>');
      return gs;
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->genset()->open(o);
    }

    static constexpr bool is_free()
    {
      return true;
    }

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Convert to a word.
    static word_t word(value_t v)
    {
      return {v};
    }

    /// Prepare to iterate over the letters of v.
    static word_t
    letters_of(word_t v)
    {
      return v;
    }

    /// Prepare to iterate over the letters of v.
    /// This is for the padded case
    static word_t
    letters_of_padded(word_t v, letter_t)
    {
      return v;
    }

    /// Prepare to iterate over v.
    static word_t
    letters_of(letter_t v)
    {
      return word(v);
    }

    /// Prepare to iterate over v.
    /// This is for the padded case
    static word_t
    letters_of_padded(letter_t v, letter_t)
    {
      return word(v);
    }


    static value_t
    special() ATTRIBUTE_PURE
    {
      return genset_t::template special<value_t>();
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t l, const value_t r)
    {
      return genset_t::equal(l, r);
    }

    /// Whether \a l < \a r.
    static bool less(const value_t l, const value_t r)
    {
      return genset_t::less(l, r);
    }

    static constexpr bool
    has_one()
    {
      return false;
    }

    static constexpr bool
    is_expressionset()
    {
      return false;
    }

    static constexpr bool
    is_letterized()
    {
      return true;
    }

    static bool
    is_special(value_t v) ATTRIBUTE_PURE
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
      return hash_value(v);
    }

    value_t
    conv(self_t, value_t v) const
    {
      VCSN_REQUIRE(is_special(v) || is_valid(v),
                   *this, ": conv: invalid label: ", str_escape(v));
      return v;
    }

    /// Convert from nullableset to letterset.
    template <typename LabelSet_>
    value_t
    conv(const nullableset<LabelSet_>& ls,
         typename nullableset<LabelSet_>::value_t v) const
    {
      require(!ls.is_one(v),
              *this, ": conv: invalid label: \\e");
      return conv(*ls.labelset(), ls.get_value(v));
    }

    /// Read one letter from i, return the corresponding label.
    value_t
    conv(std::istream& i, bool quoted = true) const
    {
      // Check for '\e', to generate a nicer error message than the
      // one from get_letter.
      if (i.good() && i.peek() == '\\')
        {
          i.ignore();
          require(i.peek() != 'e',
                  *this, ": cannot represent \\e");
          !i.unget();
        }
      return this->get_letter(i, quoted);
    }

    /// Process a label class.
    ///
    /// Stream \a i is right on a `[`.  Read up to the closing `]`,
    /// and process the labels.
    ///
    /// For instance "[a-d0-9_]".
    ///
    /// \param i    the input stream.
    /// \param fun  a (label_t) -> void function.
    template <typename Fun>
    void convs(std::istream& i, Fun fun) const
    {
      this->convs_(i, fun);
    }

    /// The longest common prefix.
    ///
    /// It would be better not to define it and to adjust
    /// tupleset::ldivide to be SNIFAE compliant, so that we get a
    /// compile time error instead of a runtime one when we try to
    /// using lgcd/ldivide on labelsets that don't support it.  However,
    /// I feel it is not nice not to get tupleset::ldivide fail to
    /// compile without a clear explanation of why, and it is quite
    /// some work to write code that supports this possible missing
    /// ldivide/lgcd (see the case of conjunction in expansionset, case
    /// of non free labelsets).
    ///
    /// Since I'm not convinced that letterset is the right
    /// abstraction (I, Akim, now tend to think that we should only
    /// support nullableset<letterset>), let's not fight this fight
    /// now.
    value_t lgcd(value_t, value_t) const
    {
      raise(*this, ": lgcd: impossible operation");
    }

    /// Compute w1 \ w2 = w1^{-1}w2.
    value_t ldivide(value_t, value_t) const
    {
      raise(*this, ": ldivide: impossible operation");
    }

    value_t conjunction(const value_t& l, const value_t& r) const
    {
      if (equal(l, r))
        return l;
      else
        raise("conjunction: invalid operation (lhs and rhs are not equal)");
    }

    std::ostream&
    print(const value_t& l, std::ostream& o = std::cout,
          format fmt = {}) const
    {
      return this->genset()->print(l, o, fmt);
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          this->genset()->print_set(o, fmt);
          break;
        case format::sname:
          o << "letterset<";
          this->genset()->print_set(o, fmt);
          o << '>';
          break;
        case format::text:
        case format::utf8:
          this->genset()->print_set(o, fmt);
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

  namespace detail
  {
    /// Conversion to letterized.
    template <typename GenSet>
    struct letterized_traits<letterset<GenSet>>
    {
      static constexpr bool is_letterized = true;

      using labelset_t = nullableset<letterset<GenSet>>;

      static labelset_t labelset(const letterset<GenSet>& ls)
      {
        return {ls.genset()};
      }
    };

    /// Conversion for letterset<GenSet> to a nullableset.
    template <typename GenSet>
    struct nullableset_traits<letterset<GenSet>>
    {
      using type = nullableset<letterset<GenSet>>;
      static type value(const letterset<GenSet>& ls)
      {
        return ls;
      }
    };

    /// Conversion for letterset<GenSet> to a super wordset.
    template <typename GenSet>
    struct law_traits<letterset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const letterset<GenSet>& ls)
      {
        return ls.genset();
      }
    };

    /*-------.
    | Join.  |
    `-------*/

    template <typename GenSet>
    struct join_impl<letterset<GenSet>, letterset<GenSet>>
    {
      using type = letterset<GenSet>;
      static type join(const letterset<GenSet>& lhs,
                       const letterset<GenSet>& rhs)
      {
        return {set_union(*lhs.genset(), *rhs.genset())};
      }
    };
  }

  /// Compute the meet with another labelset.
  template <typename GenSet>
  letterset<GenSet>
  meet(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
  {
    return {set_intersection(*lhs.genset(), *rhs.genset())};
  }
}
