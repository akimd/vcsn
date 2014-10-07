#ifndef VCSN_LABELSET_LETTERSET_HH
# define VCSN_LABELSET_LETTERSET_HH

# include <memory>

# include <vcsn/alphabets/setalpha.hh> // intersection
# include <vcsn/core/kind.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/set.hh> // intersection
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/labelset/labelset.hh>
# include <vcsn/labelset/wordset.hh>

namespace vcsn
{
  /// Implementation of labels are letters.
  template <typename GenSet>
  class letterset: public detail::genset_labelset<GenSet>
  {
  public:
    using genset_t = GenSet;
    using super_t = detail::genset_labelset<genset_t>;
    using self_type = letterset;
    using genset_ptr = std::shared_ptr<const genset_t>;

    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using letters_t = std::set<letter_t>;

    using value_t = letter_t;

    using kind_t = labels_are_letters;

    letterset(const genset_ptr& gs)
      : super_t{gs}
    {}

    letterset(const genset_t& gs = {})
      : letterset(std::make_shared<const genset_t>(gs))
    {}

    letterset(const std::initializer_list<letter_t>& letters)
      : letterset(std::make_shared<const genset_t>(letters))
    {}

    const super_t&
    super() const
    {
      return static_cast<const super_t&>(*this);
    }

    static std::string sname()
    {
      return "lal_" + super_t::sname();
    }

    std::string vname(bool full = true) const
    {
      return "lal_" + super().vname(full);
    }

    /// Build from the description in \a is.
    static letterset make(std::istream& is)
    {
      // name: lal_char(abc).
      //       ^^^ ^^^^^^^^^
      //      kind   genset
      kind_t::make(is);
      eat(is, '_');
      auto gs = genset_t::make(is);
      return gs;
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->genset().open(o);
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
    word_t word(value_t v) const
    {
      return {v};
    }

    /// Prepare to iterate over the letters of v.
    static word_t
    letters_of(word_t v)
    {
      return v;
    }

    static value_t
    special() ATTRIBUTE_PURE
    {
      return genset_t::template special<value_t>();
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

    static constexpr bool
    has_one()
    {
      return false;
    }

    static constexpr bool
    is_ratexpset()
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

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    /// Read one letter from i, return the corresponding label.
    value_t
    conv(std::istream& i) const
    {
      int c = i.peek();
      if (this->has(c))
        return i.get();
      else
        throw std::domain_error("invalid label: unexpected " + str_escape(c));
    }

    std::set<value_t>
    convs(std::istream& i) const
    {
      std::set<value_t> res;
      for (auto r : this->convs_(i))
        res.insert(value_t{r});
      return res;
    }

    std::ostream&
    print(const value_t& l, std::ostream& o,
          const std::string& = "text") const
    {
      if (!is_special(l))
        o << str_escape(l);
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        this->genset().print_set(o, format);
      else if (format == "text")
        o << vname(true);
      else
        raise("invalid format: ", format);
      return o;
    }
  };

  namespace detail
  {
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
        return {get_union(lhs.genset(), rhs.genset())};
      }
    };
  }

  /// Compute the meet with another labelset.
  template <typename GenSet>
  letterset<GenSet>
  meet(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
  {
    return {intersection(lhs.genset(), rhs.genset())};
  }
}

#endif // !VCSN_LABELSET_LETTERSET_HH
