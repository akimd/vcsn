#ifndef VCSN_LABELSET_NULLABLESET_HH
# define VCSN_LABELSET_NULLABLESET_HH

# include <cstring> //strncmp
# include <memory>
# include <set>
# include <sstream>

# include <vcsn/alphabets/setalpha.hh> // intersect
# include <vcsn/core/kind.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/labelset/labelset.hh>
# include <vcsn/labelset/fwd.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    /// Add support for an empty word to a LabelSet that does not
    /// provide such special label to this end.
    template <typename LabelSet>
    struct nullable_helper
    {
      using labelset_t = LabelSet;
      using null = nullableset<labelset_t>;
      using value_t = std::pair<typename labelset_t::value_t, bool>;
      using kind_t = typename LabelSet::kind_t;

      static null make(std::istream& is)
      {
        // name: nullableset<lal_char(abc)>.
        //                   ^^^^^^^^^^^^
        //                     labelset
        null::make_nullableset_kind(is);
        eat(is, '<');
        auto ls = null::labelset_t::make(is);
        eat(is, '>');
        return null{ls};
      }

      ATTRIBUTE_PURE
      static constexpr value_t
      one()
      {
        return value_t{null::labelset_t::special(), true};
      }

      ATTRIBUTE_PURE
      static constexpr value_t
      special()
      {
        return value_t{null::labelset_t::special(), false};
      }

      template <typename Ls>
      ATTRIBUTE_PURE
      static typename std::enable_if<Ls::has_one(), bool>::type
      is_one_(value_t l)
      {
        return std::get<1>(l) || Ls::is_one(get_value(l));
      }

      template <typename Ls>
      ATTRIBUTE_PURE
      static typename std::enable_if<!Ls::has_one(), bool>::type
      is_one_(value_t l)
      {
        return std::get<1>(l);
      }

      ATTRIBUTE_PURE
      static bool
      is_one(value_t l)
      {
        return is_one_<labelset_t>(l);
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
    template <typename GenSet>
    struct nullable_helper<letterset<GenSet>>
    {
      using labelset_t = letterset<GenSet>;
      using genset_t = GenSet;
      using null = nullableset<labelset_t>;
      using value_t = typename labelset_t::value_t;
      using kind_t = labels_are_nullable;

      static null
      make(std::istream& is)
      {
        using genset_t = typename labelset_t::genset_t;
        // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
        //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
        //        |   |    |        weightset
        //        |   |    +-- gens
        //        |   +-- letter_type
        //        +-- kind
        null::make_nullableset_kind(is);
        if (is.peek() == '_')
        {
          eat(is, '_');
          auto gs = genset_t::make(is);
          auto ls = labelset_t{gs};
          return null{ls};
        }
        eat(is, '<');
        auto ls = labelset_t::make(is);
        eat(is, '>');
        return null{ls};
      }

      ATTRIBUTE_PURE
      static constexpr typename null::value_t
      special()
      {
        return genset_t::special();
      }

      ATTRIBUTE_PURE
      static constexpr typename null::value_t
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

      template <typename... Args>
      static value_t
      value(const labelset_t& ls, Args&&... args)
      {
        return ls.value(std::forward<Args>(args)...);
      }

      ATTRIBUTE_PURE
      static typename labelset_t::value_t
      get_value(const value_t& v)
      {
        return v;
      }
    };
  }

  /// Implementation of labels are nullables (letter or empty).
  template <typename LabelSet>
  class nullableset : public LabelSet
  {
  public:
    using labelset_t = LabelSet;
    using labelset_ptr = std::shared_ptr<const labelset_t>;
    using self_type = nullableset;
    using helper_t = detail::nullable_helper<labelset_t>;
    using kind_t = typename helper_t::kind_t;

    using value_t = typename helper_t::value_t;
    using word_t = typename labelset_t::word_t;

    nullableset(const labelset_t& ls)
      : labelset_t{ls}, ls_{std::make_shared<const labelset_t>(ls)}
    {}

    nullableset(const std::shared_ptr<const labelset_t>& ls)
      : labelset_t{ls}, ls_{ls}
    {}

    static std::string sname()
    {
      return "lan<" + labelset_t::sname() + ">";
    }

    std::string vname(bool full = true) const
    {
      return "lan<" + labelset()->vname(full) + ">";
    }

    /// Build from the description in \a is.
    static nullableset make(std::istream& i)
    {
      return helper_t::make(i);
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept unknown letters
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->labelset()->open(o);
    }

    static constexpr bool
    has_one()
    {
      return true;
    }

    static constexpr bool
    is_ratexpset()
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
      return labelset()->is_valid(get_value(v)) || is_one(v);
    }

    value_t
    conv(self_type, value_t v) const
    {
      return v;
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
      return labelset()->word(get_value(l));
    }

    /// Whether l == r.
    static bool equals(const value_t& l, const value_t& r)
    {
      if (is_one(l))
        return is_one(r);
      return !is_one(r) && labelset_t::equals(get_value(l), get_value(r));
    }

    /// Whether l < r.
    static bool less_than(const value_t& l, const value_t& r)
    {
      if (is_one(r))
        return false;
      else if (is_one(l))
        return true;
      return labelset_t::less_than(get_value(l),
                                   get_value(r));
    }

    static value_t
    special()
    {
      return helper_t::special();
    }

    static bool
    is_special(const value_t& l)
    {
      return !is_one(l) && labelset_t::is_special(get_value(l));
    }

    size_t size(const value_t& v) const
    {
      return is_one(v) ? 0 : labelset_t::size(get_value(v));
    }

    static size_t hash(const value_t& v)
    {
      std::size_t res = 0;
      std::hash_combine(res, labelset_t::hash(get_value(v)));
      std::hash_combine(res, helper_t::is_one(v));
      return res;
    }

    /// \throws std::domain_error if there is no label here.
    value_t
    conv(std::istream& i) const
    {
      value_t res;
      int c = i.peek();
      if (c == '\\')
        {
          i.ignore();
          c = i.peek();
          require(c == 'e', "invalid label: unexpected \\", str_escape(c));
          i.ignore();
          res = one();
        }
      else
        res = value(ls_->conv(i));
      return res;
    }

    std::set<value_t>
    convs(std::istream& i) const
    {
      auto l = ls_->convs(i);
      std::set<value_t> res;
      for (auto v : l)
        res.emplace(value(v));
      return res;
    }

    std::ostream&
    print(value_t l, std::ostream& o,
          const std::string& format = "text") const
    {
      if (is_one(l))
        o << (format == "latex" ? "\\varepsilon" : "\\e");
      else
        labelset()->print(get_value(l), o, format);
      return o;
    }

    static void
    make_nullableset_kind(std::istream& is)
    {
      char kind[4];
      is.get(kind, sizeof kind);
      if (strncmp("lan", kind, 4))
        throw std::runtime_error("kind::make: unexpected: "
                                 + str_escape(kind)
                                 + ", expected: " + "lan");
    }

    value_t
    zero() const
    {
      return value(labelset()->zero());
    }

    bool
    is_zero(const value_t& v) const
    {
      return labelset()->is_zero(get_value(v));
    }

    bool
    is_letter(const value_t& v) const
    {
      return labelset()->is_letter(get_value(v));
    }

    value_t
    transpose(const value_t& l) const
    {
      if (is_one(l))
        return l;
      return value(ls_->transpose(get_value(l)));
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        {
          o << "(";
          labelset()->print_set(o, format);
          o << ")^?";
        }
      else if (format == "text")
        o << vname(true);
      else
        raise("invalid format: ", format);
      return o;
    }

  private:
    static typename labelset_t::value_t
    get_value(const value_t& v)
    {
      return helper_t::get_value(v);
    }

    labelset_ptr ls_;
  };

  namespace detail
  {
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

#define DEFINE(Lhs, Rhs)                                \
    template <typename GenSet>                          \
    struct join_impl<Lhs, Rhs>                          \
    {                                                   \
      using type = Rhs;                                 \
      static type join(const Lhs& lhs, const Rhs& rhs)  \
      {                                                 \
        return {get_union(lhs.genset(), rhs.genset())}; \
      }                                                 \
    }

    /// The join with another labelset.
    DEFINE(letterset<GenSet>,              nullableset<letterset<GenSet>>);
    DEFINE(nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);
#undef DEFINE

  }

  /*-------.
  | Meet.  |
  `-------*/

#define DEFINE(Lhs, Rhs, Res)                                 \
  template <typename GenSet>                                  \
  Res                                                         \
  meet(const Lhs& lhs, const Rhs& rhs)                        \
  {                                                           \
    return {intersection(lhs.genset(), rhs.genset())};        \
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
    return nullableset<meet_t<Lls, Rls>>{meet(*lhs.labelset(), *rhs.labelset())};
  }

#undef DEFINE

}

#endif // !VCSN_LABELSET_NULLABLESET_HH
