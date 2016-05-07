#pragma once

#include <memory>
#include <set>

#include <boost/range/algorithm/mismatch.hpp>
#include <boost/algorithm/string/predicate.hpp> // starts_with

#include <vcsn/core/kind.hh>
#include <vcsn/labelset/fwd.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/letterset.hh> // for letterized_traits
#include <vcsn/labelset/genset-labelset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// Implementation of labels are words.
  template <typename GenSet>
  class wordset: public detail::genset_labelset<GenSet>
  {
  public:
    using genset_t = GenSet;
    using super_t = detail::genset_labelset<genset_t>;
    using self_t = wordset;
    using genset_ptr = std::shared_ptr<const genset_t>;

    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using letters_t = std::set<letter_t>;

    using value_t = word_t;

    using kind_t = labels_are_words;

    wordset(const genset_ptr& gs)
      : super_t{gs}
    {}

    wordset(const genset_t& gs = {})
      : wordset{std::make_shared<const genset_t>(gs)}
    {}

    wordset(std::initializer_list<letter_t> letters)
      : wordset(std::make_shared<const genset_t>(letters))
    {}

    static symbol sname()
    {
      static auto res = symbol{"wordset<" + super_t::sname() + '>'};
      return res;
    }

    /// Build from the description in \a is.
    static wordset make(std::istream& is)
    {
      // name: wordset<char_letters(abc)>.
      //       ^^^^^^^ ^^^^^^^^^^^^^^^^^
      //         kind      genset
      eat(is, "wordset<");
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
      return false;
    }

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Convert to a word.
    word_t word(const value_t& v) const
    {
      return v;
    }

    /// Prepare to iterate over the letters of v.
    static word_t
    letters_of(const value_t& v)
    {
      return v;
    }

    /// Prepare to iterate over the letters of v.
    /// This is for the padded case
    static word_t
    letters_of_padded(const value_t& v, letter_t)
    {
      return v;
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t& l, const value_t& r)
    {
      return l == r;
    }

    /// Whether \a l < \a r.
    static bool less(const letter_t& l, const letter_t& r)
    {
      return genset_t::less(l, r);
    }

    /// Whether \a l < \a r.
    static bool less(const value_t& l, const value_t& r)
    {
      // Be sure to use genset::less().
      auto s1 = size(l);
      auto s2 = size(r);
      if (s1 < s2)
        return true;
      else if (s2 < s1)
        return false;
      else
        return genset_t::less(l, r);
    }

    static value_t
    special()
    {
      return genset_t::template special<value_t>();
    }

    static bool
    is_special(const value_t& v)
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
    is_expressionset()
    {
      return false;
    }

    static constexpr bool
    has_one()
    {
      return true;
    }

    static constexpr bool
    is_letterized()
    {
      return false;
    }

    static value_t
    one()
    {
      return genset_t::empty_word();
    }

    static bool
    is_one(const value_t& l) ATTRIBUTE_PURE
    {
      return genset_t::is_empty_word(l);
    }

    static size_t size(const value_t& v)
    {
      // Not v.length(), because word_t can actually be a vector
      // (e.g., with string_letters).
      return v.size();
    }

    static size_t hash(const value_t& v)
    {
      return hash_value(v);
    }

    value_t
    conv(self_t, const value_t& v) const
    {
      return v;
    }

    template <typename GenSet_>
    value_t
    conv(const letterset<GenSet_>& ls,
         typename letterset<GenSet_>::value_t v) const
    {
      if (ls.is_special(v))
        return special();
      else
        {
          auto res = value(v);
          VCSN_REQUIRE(is_valid(res),
                       *this, ": conv: invalid label: ", str_escape(v));
          return res;
        }
    }

    template <typename LabelSet_>
    value_t
    conv(const nullableset<LabelSet_>& ls,
         const typename nullableset<LabelSet_>::value_t& v) const
    {
      if (ls.is_one(v))
        return one();
      else
        return conv(*ls.labelset(), ls.get_value(v));
    }

    /// Read a word from this stream.
    value_t
    conv(std::istream& i, bool = true) const
    {
      return this->genset()->get_word(i);
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
      this->convs_(i, [this,fun](letter_t l) { fun(value(l)); });
    }

    std::ostream&
    print(const value_t& l, std::ostream& o = std::cout,
          format fmt = {}) const
    {
      if (is_one(l))
        o << (fmt == format::latex ? "\\varepsilon"
              : fmt == format::utf8 ? "ε"
              : "\\e");
      else if (!is_special(l))
        this->genset()->print(l, o, fmt);
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          this->genset()->print_set(o, fmt);
          o << "^*";
          break;
        case format::sname:
          o << "wordset<";
          this->genset()->print_set(o, fmt);
          o << '>';
          break;
        case format::text:
        case format::utf8:
          this->genset()->print_set(o, fmt);
          o << '*';
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }

    /// The longest common prefix.
    static value_t lgcd(const value_t& w1, const value_t& w2)
    {
      return {w1.begin(), boost::mismatch(w1, w2).first};
    }

    /// Compute w1 \ w2 = w1^{-1}w2.
    /// Precondition: w1 is prefix of w2.
    static value_t ldiv(const value_t& w1, const value_t& w2)
    {
      using boost::algorithm::starts_with;
      VCSN_REQUIRE(starts_with(w2, w1),
                   sname(), ": ldiv: invalid arguments: ", str_escape(w1),
                   ", ", str_escape(w2));
      return {begin(w2) + size(w1), end(w2)};
    }

    /// w2 := w1 \ w2.
    static value_t& ldiv_here(const value_t& w1, value_t& w2)
    {
      w2 = ldiv(w1, w2);
      return w2;
    }

    const value_t& conjunction(const value_t& l, const value_t& r) const
    {
      if (equal(l, r))
        return l;
      else
        raise("conjunction: invalid operation (lhs and rhs are not equal)");
    }
  };

  namespace detail
  {
    /// Conversion to letterized.
    template <typename GenSet>
    struct letterized_traits<wordset<GenSet>>
    {
      static constexpr bool is_letterized = false;

      using labelset_t = nullableset<letterset<GenSet>>;

      static labelset_t labelset(const wordset<GenSet>& ls)
      {
        return {ls.genset()};
      }
    };

    /// wordset is already a nullableset.
    template <typename GenSet>
    struct nullableset_traits<wordset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const wordset<GenSet>& ls)
      {
        return ls;
      }
    };

    template <typename GenSet>
    struct law_traits<wordset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const wordset<GenSet>& ls)
      {
        return ls;
      }
    };

    /*-------.
    | Join.  |
    `-------*/

    /// Declare that Lhs v Rhs => Rhs (on the union of alphabets).
#define DEFINE(Lhs, Rhs)                                  \
    template <typename GenSet>                            \
    struct join_impl<Lhs, Rhs>                            \
    {                                                     \
      using type = Rhs;                                   \
      static type join(const Lhs& lhs, const Rhs& rhs)    \
      {                                                   \
        return {set_union(*lhs.genset(), *rhs.genset())}; \
      }                                                   \
    }

    /// The join with another labelset.
    DEFINE(letterset<GenSet>,              wordset<GenSet>);
    DEFINE(nullableset<letterset<GenSet>>, wordset<GenSet>);
    DEFINE(wordset<GenSet>,                wordset<GenSet>);
#undef DEFINE
  }

  /// Compute the meet with another alphabet.
  // FIXME: Factor in genset_labelset?
  template <typename GenSet>
  wordset<GenSet>
  meet(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
  {
    return {set_intersection(*lhs.genset(), *rhs.genset())};
  }
}
