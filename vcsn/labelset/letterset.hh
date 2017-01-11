#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <vcsn/alphabets/setalpha.hh> // intersection
#include <vcsn/core/kind.hh>
#include <vcsn/labelset/genset-labelset.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/wordset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/set.hh> // intersection

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

    using value_t = letter_t;
    using values_t = std::set<value_t, vcsn::less<self_t>>;

    using kind_t = labels_are_letters;

    using super_t::mul;

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

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Convert to a word.
    static word_t word(value_t v)
    {
      if (is_one(v))
        return make_wordset(self_t{}).one();
      else
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

    static constexpr value_t
    one() ATTRIBUTE_PURE
    {
      return genset_t::one_letter();
    }

    static value_t
    special() ATTRIBUTE_PURE
    {
      return genset_t::template special<value_t>();
    }

    /// Three-way comparison between \a l and \a r.
    static int compare(const value_t l, const value_t r)
    {
      if (auto res = int(is_one(r)) - int(is_one(l)))
        return res;
      else
        return genset_t::compare(l, r);
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t l, const value_t r)
    {
      if (is_one(l))
        return is_one(r);
      else
        return !is_one(r) && genset_t::equal(l, r);
    }

    /// Whether \a l < \a r.
    static bool less(const value_t l, const value_t r)
    {
      if (is_one(r))
        return false;
      else if (is_one(l))
        return true;
      else
        return genset_t::less(l, r);
    }

    /// The concatenation.
    value_t mul(const value_t l, const value_t r) const
    {
      if (is_one(r))
        return l;
      else if (is_one(l))
        return r;
      else
        super_t::mul(l, r);
    }

    static constexpr bool
    has_one()
    {
      return true;
    }

    static constexpr bool
    is_expressionset()
    {
      return false;
    }

    bool
    is_letter(const value_t v) const
    {
      return !is_one(v) && super_t::is_letter(v);
    }

    static constexpr bool
    is_letterized()
    {
      return true;
    }

    static bool
    is_special(value_t v) ATTRIBUTE_PURE
    {
      return !is_one(v) && v == special();
    }

    static constexpr bool
    is_one(value_t v)
    {
      return v == one();
    }

    bool
    is_valid(value_t v) const
    {
      return is_one(v) || this->has(v);
    }

    static size_t size(value_t v)
    {
      return !is_one(v);
    }

    // FIXME: specialize for both implementation.
    static size_t hash(value_t v)
    {
      // Do not use get_value when is_one.  Let's hash one() as 0.
      return is_one(v) ? 0 : hash_value(v);
    }

    value_t
    conv(self_t, value_t v) const
    {
      VCSN_REQUIRE(is_special(v) || is_valid(v),
                   *this, ": conv: invalid label: ", str_escape(v));
      return v;
    }

    value_t
    conv(oneset, typename oneset::value_t) const
    {
      return one();
    }

    /// Read one letter from i, return the corresponding label.
    value_t
    conv(std::istream& i, bool quoted = true) const
    {
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

    // FIXME: Nullableset removal: update comment.
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
    value_t lgcd(const value_t l, const value_t r) const
    {
      if (equal(l, r))
        return l;
      else if (is_one(l) || is_one(r))
        return one();
      else
        raise(*this, ": lgcd: invalid arguments: ",
          to_string(*this, l), ", ", to_string(*this, r));
    }

    /// Compute l \ r = l^{-1}r.
    value_t ldivide(const value_t l, const value_t r) const
    {
      if (auto res = maybe_ldivide(l, r))
        return *res;
      else
        raise(*this, ": ldivide: invalid arguments: ",
              to_string(*this, l), ", ", to_string(*this, r));
    }

    boost::optional<value_t>
    maybe_ldivide(const value_t l, const value_t r) const
    {
      if (equal(l, r))
        return one();
      else if (is_one(l))
        return r;
      else if (is_one(r))
        return boost::none;
      else
        raise(*this, ": maybe_ldivide: invalid arguments: ",
              to_string(*this, l), ", ", to_string(*this, r));
    }

    /// Compute l / r.
    value_t rdivide(const value_t l, const value_t r) const
    {
      if (auto res = maybe_rdivide(l, r))
        return *res;
      else
        raise(*this, ": rdivide: invalid arguments: ",
              to_string(*this, l), ", ", to_string(*this, r));
    }

    boost::optional<value_t>
    maybe_rdivide(const value_t l, const value_t r) const
    {
      if (equal(l, r))
        return one();
      else if (is_one(l))
        return boost::none;
      else if (is_one(r))
        return l;
      else
        raise(*this, ": maybe_rdivide: invalid arguments: ",
              to_string(*this, l), ", ", to_string(*this, r));
    }


    value_t conjunction(const value_t l, const value_t r) const
    {
      if (is_one(l) && is_one(r))
        return l;
      else if (!is_one(l) && !is_one(r) && equal(l, r))
        return l;
      else
        raise(*this,
              ": conjunction: invalid operation (lhs and rhs are not equal): ",
              to_string(*this, l), ", ", to_string(this, r));
    }

    std::ostream&
    print(const value_t& l, std::ostream& o = std::cout,
          format fmt = {}) const
    {
      if (is_one(l))
        o << (fmt == format::latex ? "\\varepsilon"
              : fmt == format::utf8 ? "ε"
              : "\\e");
      else
        this->genset()->print(l, o, fmt);
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << '(';
          this->genset()->print_set(o, fmt);
          o << ")^?";
          break;
        case format::sname:
          o << "letterset<";
          this->genset()->print_set(o, fmt);
          o << '>';
          break;
        case format::text:
        case format::utf8:
          this->genset()->print_set(o, fmt);
          o << '?';
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

  /*-------.
  | Meet.  |
  `-------*/

  /// Compute the meet with another labelset.
  template <typename GenSet>
  letterset<GenSet>
  meet(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
  {
    return {set_intersection(*lhs.genset(), *rhs.genset())};
  }

  /*----------------.
  | random_label.   |
  `----------------*/

  /// Random label from letterset.
  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename letterset<GenSet>::value_t
  random_label(const letterset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    // FIXME: the proportion should be controllable.
    auto dis = std::bernoulli_distribution(0.5);
    if (dis(gen) || ls.generators().empty())
      return ls.one();
    else
      {
        // Pick a member of a container following a uniform distribution.
        auto pick = make_random_selector(gen);
        return ls.value(pick(ls.generators()));
      }
  }
}
