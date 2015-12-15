#pragma once

#include <memory>
#include <set>
#include <sstream>

#include <vcsn/alphabets/setalpha.hh> // intersect
#include <vcsn/core/kind.hh>
#include <vcsn/labelset/fwd.hh>
#include <vcsn/labelset/genset-labelset.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    /// Add support for an empty word to a LabelSet that does not
    /// provide such special label to this end.
    ///
    /// \tparam LabelSet
    ///    the LabelSet to which we want to add nullable support
    ///    (e.g., a letterset<>).
    template <typename LabelSet>
    struct nullable_helper
    {
      using labelset_t = LabelSet;
      using value_t = std::pair<typename labelset_t::value_t, bool>;

      ATTRIBUTE_PURE
      static constexpr value_t
      one()
      {
        return value_t{labelset_t::special(), true};
      }

      ATTRIBUTE_PURE
      static constexpr value_t
      special()
      {
        return value_t{labelset_t::special(), false};
      }

      template <typename Ls>
      ATTRIBUTE_PURE
      static vcsn::enable_if_t<Ls::has_one(), bool>
      is_one_(const value_t& l)
      {
        return std::get<1>(l) || Ls::is_one(get_value(l));
      }

      template <typename Ls>
      ATTRIBUTE_PURE
      static vcsn::enable_if_t<!Ls::has_one(), bool>
      is_one_(const value_t& l)
      {
        return std::get<1>(l);
      }

      ATTRIBUTE_PURE
      static bool
      is_one(const value_t& l)
      {
        return is_one_<labelset_t>(l);
      }

      template <typename Value>
      static Value
      transpose(const labelset_t& ls, Value l)
      {
        // This is not robust enough: Value is word_t, then it's not
        // valid to call is_one on it.  Since I'm unsure of the future
        // of nullableset, let's not waste time on this now.
        if (is_one(l))
          return l;
        else
          return {ls.transpose(get_value(l)), false};
      }

      template <typename... Args>
      static value_t
      value(const labelset_t& ls, Args&&... args)
      {
        return {ls.value(std::forward<Args>(args)...), false};
      }

      ATTRIBUTE_PURE
      static typename labelset_t::value_t
      get_value(const value_t& v)
      {
        return std::get<0>(v);
      }
    };

    /// Add support for an empty word to a letterset thanks to the
    /// one() of its genset.
    ///
    /// \tparam GenSet
    ///    the GenSet of the LabelSet to which we want to add nullable
    ///    support (i.e., letterset<GenSet>).
    template <typename GenSet>
    struct nullable_helper<letterset<GenSet>>
    {
      using genset_t = GenSet;
      using labelset_t = letterset<genset_t>;
      using value_t = typename labelset_t::value_t;

      ATTRIBUTE_PURE
      static constexpr value_t
      special()
      {
        return genset_t::special();
      }

      ATTRIBUTE_PURE
      static constexpr value_t
      one()
      {
        return genset_t::one_letter();
      }

      ATTRIBUTE_PURE
      static bool
      is_one(value_t l)
      {
        return l == one();
      }

      template <typename Value>
      static Value
      transpose(const labelset_t& ls, const Value& l)
      {
        return ls.transpose(l);
      }

      template <typename... Args>
      static value_t
      value(const labelset_t& ls, Args&&... args)
      {
        return ls.value(std::forward<Args>(args)...);
      }

      ATTRIBUTE_PURE
      static value_t
      get_value(const value_t& v)
      {
        return v;
      }
    };
  }

  /// Implementation of labels are nullables (letter or empty).
  template <typename LabelSet>
  class nullableset
  {
  public:
    using labelset_t = LabelSet;
    using labelset_ptr = std::shared_ptr<const labelset_t>;
    using self_t = nullableset;
    using helper_t = detail::nullable_helper<labelset_t>;
    using kind_t = labels_are_nullable;

    using value_t = typename helper_t::value_t;
    using letter_t = typename labelset_t::letter_t;
    using word_t = typename labelset_t::word_t;

  private:
    /// The wrapped LabelSet.
    /// Declared early to please decltype.
    labelset_ptr ls_;

  public:
    using genset_ptr = decltype(ls_->genset());
    using genset_t = decltype(ls_->generators());

  public:
    nullableset(const labelset_ptr& ls)
      : ls_{ls}
    {}

    nullableset(const labelset_t& ls = {})
      : nullableset{std::make_shared<const labelset_t>(ls)}
    {}

    static symbol sname()
    {
      static auto res = symbol{"nullableset<" + labelset_t::sname() + '>'};
      return res;
    }

    /// Build from the description in \a is.
    static nullableset make(std::istream& is)
    {
      // name: nullableset<lal_char(abc)>.
      //                   ^^^^^^^^^^^^
      //                     labelset
      eat(is, "nullableset<");
      auto ls = labelset_t::make(is);
      eat(is, '>');
      return {ls};
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept unknown letters
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->ls_->open(o);
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

    static constexpr bool
    is_letterized()
    {
      return labelset_t::is_letterized();
    }

    static constexpr bool is_free()
    {
      return false;
    }

    ATTRIBUTE_PURE
    static constexpr value_t
    one()
    {
      return helper_t::one();
    }

    ATTRIBUTE_PURE
    static bool
    is_one(value_t l)
    {
      return helper_t::is_one(l);
    }

    bool
    is_valid(value_t v) const
    {
      return is_one(v) || ls_->is_valid(get_value(v));
    }

    genset_ptr
    genset() const
    {
      return ls_->genset();
    }

    /// The generators.  Meaningful for labelsets only.
    genset_t
    generators() const
    {
      return ls_->generators();
    }

    value_t
    conv(self_t, value_t v) const
    {
      return v;
    }

    value_t
    conv(oneset, typename oneset::value_t) const
    {
      return one();
    }

    /// Conversion from another type: first by the wrapped labelset,
    /// and then by our wrappers (in case the wrapped labelset does
    /// not support "one").
    template <typename LabelSet_>
    value_t
    conv(const LabelSet_& ls, typename LabelSet_::value_t v) const
    {
      return value(ls_->conv(ls, v));
    }

    const labelset_ptr labelset() const
    {
      return ls_;
    }

    template <typename... Args>
    value_t
    value(Args&&... args) const
    {
      return helper_t::value(*ls_, std::forward<Args>(args)...);
    }

    word_t
    word(const value_t& l) const
    {
      if (is_one(l))
        return make_wordset(*labelset()).one();
      else
        return ls_->word(get_value(l));
    }

    /// Prepare to iterate over the letters of v.
    auto static
    letters_of(const word_t& v)
      -> decltype(labelset_t::letters_of(v))
    {
      return labelset_t::letters_of(v);
    }

    /// Prepare to iterate over the letters of v.
    /// This is for the padded case
    auto
    letters_of_padded(const word_t& v, letter_t l) const
      -> decltype(ls_->letters_of_padded(v, l))
    {
      return ls_->letters_of_padded(v, l);
    }

    auto
    letters_of_padded(value_t v, letter_t l) const
      -> decltype(this->letters_of_padded(this->word(v), l))
    {
      return letters_of_padded(word(v), l);
    }

    /// Whether l == r.
    static bool equal(const value_t& l, const value_t& r)
    {
      if (is_one(l))
        return is_one(r);
      else
        return !is_one(r) && labelset_t::equal(get_value(l), get_value(r));
    }

    /// Whether l < r.
    static bool less(const value_t& l, const value_t& r)
    {
      if (is_one(r))
        return false;
      else if (is_one(l))
        return true;
      else
        return labelset_t::less(get_value(l), get_value(r));
    }

    /// The longest common prefix.
    value_t lgcd(const value_t& l, const value_t& r) const
    {
      if (equal(l, r))
        return l;
      else if (is_one(l) || is_one(r))
        return one();
      else
        return value(ls_->lgcd(get_value(l), get_value(r)));
    }

    /// Compute l \ r = l^{-1}r.
    value_t ldiv(const value_t& l, const value_t& r) const
    {
      if (equal(l, r))
        return one();
      else if (is_one(l))
        return r;
      else if (is_one(r))
        raise(*this, ": ldiv: invalid arguments: ",
              to_string(*this, l), ", ", to_string(*this, r));
      else
        return value(ls_->ldiv(get_value(l), get_value(r)));
    }

    static value_t
    special()
    {
      return helper_t::special();
    }

    static bool
    is_special(const value_t& v)
    {
      return !is_one(v) && labelset_t::is_special(get_value(v));
    }

    static size_t size(const value_t& v)
    {
      return is_one(v) ? 0 : labelset_t::size(get_value(v));
    }

    // FIXME: specialize for both implementation.
    static size_t hash(const value_t& v)
    {
      // Do not use get_value when is_one.  Let's hash one() as 0.
      return is_one(v) ? 0 : labelset_t::hash(get_value(v));
    }

    letter_t
    get_letter(std::istream& i, bool quoted = true) const
    {
      return ls_->get_letter(i, quoted);
    }

    /// Read a label from a stream.
    value_t
    conv(std::istream& i, bool quoted = true) const
    {
      // Check for '\e', otherwise pass it to the inner labelset.
      if (i.good() && i.peek() == '\\')
        {
          i.ignore();
          int c = i.peek();
          if (c == 'e')
            {
              i.ignore();
              return {};
            }
          else
            i.unget();
        }
      return value(ls_->conv(i, quoted));
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
    void convs(std::istream& i, Fun&& fun) const
    {
      ls_->convs(i,
                 [this,fun](const typename labelset_t::value_t& l)
                 {
                   fun(value(l));
                 });
    }

    value_t conjunction(const value_t& l, const value_t& r) const
    {
      if (is_one(l) && is_one(r))
        return l;
      else if (!is_one(l) && !is_one(r))
        return ls_->conjunction(get_value(l), get_value(r));
      else
        raise("conjunction: invalid operation (lhs and rhs are not equal)");
    }

    /// Print label to stream.
    std::ostream&
    print(const value_t& l, std::ostream& o,
          format fmt = {}) const
    {
      if (is_one(l))
        o << (fmt == format::latex ? "\\varepsilon"
              : fmt == format::utf8 ? "ε"
              : "\\e");
      else
        ls_->print(get_value(l), o, fmt);
      return o;
    }

    value_t
    zero() const
    {
      return value(ls_->zero());
    }

    bool
    is_zero(const value_t& v) const
    {
      return !is_one(v) && ls_->is_zero(get_value(v));
    }

    bool
    is_letter(const value_t& v) const
    {
      return !is_one(v) && ls_->is_letter(get_value(v));
    }

    /// Mirror label.
    template <typename Value>
    Value
    transpose(const Value& l) const
    {
      return helper_t::transpose(*ls_, l);
    }

    /// Print labelset description.
    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "(";
          ls_->print_set(o, fmt);
          o << ")^?";
          break;

        case format::sname:
          o << "nullableset<";
          ls_->print_set(o, fmt);
          o << '>';
          break;

        case format::text:
        case format::utf8:
          ls_->print_set(o, fmt);
          o << '?';
          break;

        case format::raw:
          assert(0);
          break;
        }

      return o;
    }

    /// The (inner) value when it (the outer value) is not one.
    static typename labelset_t::value_t
    get_value(const value_t& v)
    {
      assert(!is_one(v));
      return helper_t::get_value(v);
    }
  };

  namespace detail
  {
    /// Conversion to letterized.
    template <typename LabelSet>
    struct letterized_traits<nullableset<LabelSet>>
    {
      using traits = letterized_traits<LabelSet>;
      static constexpr bool is_letterized = traits::is_letterized;

      using labelset_t = typename traits::labelset_t;

      static labelset_t labelset(const nullableset<LabelSet>& ls)
      {
        return make_letterized(*ls.labelset());
      }
    };

    /// Conversion for nullable<LabelSet> to a nullableset: id.
    template <typename LabelSet>
    struct nullableset_traits<nullableset<LabelSet>>
    {
      using type = nullableset<LabelSet>;
      static type value(const nullableset<LabelSet>& ls)
      {
        return ls;
      }
    };

    template <typename LabelSet>
    struct proper_traits<nullableset<LabelSet>>
    {
      using type = LabelSet;
      static type value(const nullableset<LabelSet>& ls)
      {
        return *ls.labelset();
      }
    };

    /// Conversion for nullable<LabelSet> to a super wordset.
    template <typename LabelSet>
    struct law_traits<nullableset<LabelSet>>
    {
      using type = law_t<LabelSet>;
      static type value(const nullableset<LabelSet>& ls)
      {
        return make_wordset(*ls.labelset());
      }
    };

    /*-------.
    | Join.  |
    `-------*/

    /// `oneset v LS -> nullableset<LS>` if \c !LS\::has_one.
    template <typename LS>
    struct join_impl<oneset, LS,
                     enable_if_t<!LS::has_one()>>
    {
      using type = nullableset<LS>;
      static type join(const oneset&, const LS& ls)
      {
        return {ls};
      }
    };

    /// `oneset v LS -> LS` if \c LS\::has_one.
    template <typename LS>
    struct join_impl<oneset, LS,
                     enable_if_t<LS::has_one()>>
    {
      using type = LS;
      static type join(const oneset&, const LS& ls)
      {
        return ls;
      }
    };

    /// The join with another labelset.
    template <typename LS1, typename LS2>
    struct join_impl<nullableset<LS1>, LS2>
    {
      using type = nullableset<join_t<LS1, LS2>>;
      static type join(const nullableset<LS1>& ls1, const LS2& ls2)
      {
        return {::vcsn::join(*ls1.labelset(), ls2)};
      }
    };

    /// The join with another labelset.
    ///
    /// We specialize the previous case to avoid nesting nullableset
    /// inside nullableset.
    template <typename LS1, typename LS2>
    struct join_impl<nullableset<LS1>, nullableset<LS2>>
    {
      using type = nullableset<join_t<LS1, LS2>>;
      static type join(const nullableset<LS1>& ls1,
                       const nullableset<LS2>& ls2)
      {
        return {::vcsn::join(*ls1.labelset(), *ls2.labelset())};
      }
    };
  }

  /*-------.
  | Meet.  |
  `-------*/

#define DEFINE(Lhs, Rhs, Res)                                 \
  template <typename GenSet>                                  \
  Res                                                         \
  meet(const Lhs& lhs, const Rhs& rhs)                        \
  {                                                           \
    return {intersection(*lhs.genset(), *rhs.genset())};      \
  }

  /// Compute the meet with another labelset.
  DEFINE(nullableset<letterset<GenSet>>,
         nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);

  DEFINE(letterset<GenSet>,
         nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);

  DEFINE(nullableset<letterset<GenSet>>,
         letterset<GenSet>, nullableset<letterset<GenSet>>);

  template <typename Lls, typename Rls>
  nullableset<meet_t<Lls, Rls>>
  meet(const nullableset<Lls>& lhs, const nullableset<Rls>& rhs)
  {
    return nullableset<meet_t<Lls, Rls>>{meet(*lhs.labelset(),
                                              *rhs.labelset())};
  }

#undef DEFINE

}
