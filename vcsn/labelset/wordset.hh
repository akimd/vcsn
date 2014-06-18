#ifndef VCSN_LABELSET_WORDSET_HH
# define VCSN_LABELSET_WORDSET_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/labelset/labelset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// Implementation of labels are words.
  template <typename GenSet>
  class wordset: public detail::genset_labelset<GenSet>
  {
  public:
    using genset_t = GenSet;
    using super_type = detail::genset_labelset<genset_t>;
    using self_type = wordset;
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
      // name: law_char(abc).
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
    letters_of(word_t v)
    {
      return v;
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
    is_ratexpset()
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
      return v.length();
    }

    static size_t hash(const value_t& v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    // FIXME: Why do I need to repeat this?
    // It should be inherited from genset-labelset.
    value_t
    conv(std::istream& i) const
    {
      return this->genset().conv(i);
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
          const std::string& format = "text") const
    {
      if (is_one(l))
        o << (format == "latex" ? "\\varepsilon" : "\\e");
      else if (!is_special(l))
        o << str_escape(l);
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      if (format == "latex")
        {
          this->genset().print_set(o, format);
          o << "^*";
        }
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
    struct law_traits<wordset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const wordset<GenSet>& ls)
      {
        return ls;
      }
    };
  }

  /// Compute the meet with another alphabet.
  // FIXME: Factor in genset_labelset?
  template <typename GenSet>
  wordset<GenSet>
  meet(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
  {
    return {intersection(lhs.genset(), rhs.genset())};
  }

  /// Compute the union with another alphabet.
  template <typename GenSet>
  wordset<GenSet>
  join(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
  {
    return {get_union(lhs.genset(), rhs.genset())};
  }
}

#endif // !VCSN_LABELSET_WORDSET_HH
